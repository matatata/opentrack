#include "wine/unixlib.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(npclient);

enum unix_funcs
{
    np_get_ft_data = 0,
    np_register_program_profile_id
};
