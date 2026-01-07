#include <cstdint>
#include <vector>
#include <cmath>

namespace Asterix240 {

// Radar PI typically uses 4096 spokes per rotation
static constexpr int SPOKES_PER_ROTATION = 4096;

// Convert degrees to spoke index
inline int AzimuthDegToSpoke(float az_deg) {
    if (az_deg < 0.0f)
        az_deg += 360.0f;

    int spoke = static_cast<int>(
        (az_deg / 360.0f) * SPOKES_PER_ROTATION
    ) % SPOKES_PER_ROTATION;

    return spoke;
}

// Convert intensity (CAT-240 often 0–255 already)
inline uint8_t NormalizeIntensity(uint16_t value) {
    if (value > 255)
        return 255;
    return static_cast<uint8_t>(value);
}

// Simple container for a decoded CAT-240 spoke
struct VideoSpoke {
    int spoke_index;                 // 0..4095
    uint32_t timestamp_ms;           // monotonic or epoch
    float start_range_m;             // meters
    float cell_size_m;               // meters per bin
    std::vector<uint8_t> bins;       // intensity values
};

} // namespace Asterix240
