#include <caller/middleware/crc_pipeline_stage.hpp>
#include <caller/context/context.hpp>
#include <mutex>

CALLER_BEGIN

PipelineStageWrapper::Handler CRCPipelineStage::validate16Intercept(size_t start, size_t end)
{
    if (start >= end) {
        return nullptr;
    }

    return [start, end](IOContextPtr context,ByteBuffer data,
            ServicePtr service, PipelineStagePtr next) {

        if (data.length() <= start || data.length() < end + sizeof(uint16_t)) {
            return;
        }

        uint16_t rcrc = 0;
        if (!data.take(rcrc, end , sizeof(uint16_t))) {
            return;
        }

        const char *rawdata   = data.data();
        const char *startData = &rawdata[start];
        const char *endData   = &rawdata[end];

        uint16_t crc  = calculateCRC(startData, static_cast<int>(endData - startData));
        if (rcrc == crc) {
            next->propagate(context, data, service);
        }
    };
}

uint16_t CRCPipelineStage::calculateCRC(const char *input, int size)
{
    static bool crc_tabccitt_init = false;
    static uint16_t crc_tabccitt[256];
    static std::once_flag once;

    int   a;
    uint16_t crc;
    uint16_t tmp;
    uint16_t short_c;
    const unsigned char *ptr;

    std::call_once(once, []() {
        uint16_t i;
        uint16_t j;
        uint16_t crc;
        uint16_t c;
        for (i=0; i<256; i++) {
            crc = 0;
            c   = static_cast<uint16_t>(i << 8);
            for (j=0; j<8; j++) {
                if ( (crc ^ c) & 0x8000 ) {
                    crc = static_cast<uint16_t>((crc << 1 ) ^ 0x1021);
                }else{
                    crc = static_cast<uint16_t>(crc << 1);
                }
                c = static_cast<uint16_t>(c << 1);
            }
            crc_tabccitt[i] = crc;
        }
        crc_tabccitt_init = true;
    });

    crc = 0;
    ptr = reinterpret_cast<const unsigned char*>(input);
    if ( ptr != nullptr ){
        for (a = 0; a < size; a++) {
            short_c = 0x00ff & static_cast<unsigned short>(*ptr);
            tmp     = (crc >> 8) ^ short_c;
            crc     = static_cast<uint16_t>((crc << 8) ^ crc_tabccitt[tmp]);
            ptr++;
        }
    }

    return crc;
}

CALLER_END
