#ifdef INITIALIZE_RADAR

PLUGIN_BEGIN_NAMESPACE

/*
 * ASTERIX CAT-240
 *
 * Multicast-only receive radar
 * - report_addr : multicast group + port
 * - interface_addr : selected NIC
 */

// Default CAT-240 multicast group (can be overridden in UI later)
static const NetworkAddress g_asterix_report(224, 0, 2, 2, 40202);
static const NetworkAddress g_asterix_radar_interface(192, 168, 2, 1, 0);

PLUGIN_END_NAMESPACE

#endif

// ---------------- Range tables ----------------
#define RANGE_METRIC_RT_ASTERIX240                                            \
    { 250, 500, 750, 1000, 1500, 2000, 3000, 5000, 7500, 10000, 15000, 20000 }

#define RANGE_NAUTIC_RT_ASTERIX240                                            \
    { 1852 / 8, 1852 / 4, 1852 / 2, 1852, 1852 * 2, 1852 * 4, 1852 * 8,        \
      1852 * 12, 1852 * 16, 1852 * 24, 1852 * 36, 1852 * 48 }

#define RANGE_MIXED_RT_ASTERIX240                                             \
    { 1852 / 8, 1852 / 4, 1852 / 2, 1852, 1852 * 2, 1852 * 4, 1852 * 8,        \
      1852 * 12, 1852 * 16, 1852 * 24, 1852 * 36, 1852 * 48 }

// ---------------- Geometry ----------------

// CAT-240 commonly supports very high angular resolution
#define ASTERIX240_SPOKES 4096
#define ASTERIX240_MAX_SPOKE_LEN 4096

#if SPOKES_MAX < ASTERIX240_SPOKES
#undef SPOKES_MAX
#define SPOKES_MAX ASTERIX240_SPOKES
#endif

#if SPOKE_LEN_MAX < ASTERIX240_MAX_SPOKE_LEN
#undef SPOKE_LEN_MAX
#define SPOKE_LEN_MAX ASTERIX240_MAX_SPOKE_LEN
#endif

// ---------------- Radar registration ----------------

DEFINE_RADAR(
    RT_ASTERIX240,                     /* Radar type enum */
    wxT("ASTERIX CAT-240"),            /* Display name */
    ASTERIX240_SPOKES,                 /* Spokes */
    ASTERIX240_MAX_SPOKE_LEN,          /* Spoke length */
    Asterix240ControlsDialog,          /* Controls dialog (placeholder) */
    Asterix240Receive(
        pi,
        ri,
        g_asterix_report,              /* Multicast group */
        g_asterix_radar_interface
    ),
    Asterix240Control,                 /* Control (dummy) */
    0                                  /* Primary */
)
