
typedef enum npclient_status_ {
    NPCLIENT_STATUS_OK,
    NPCLIENT_STATUS_DISABLED
} npclient_status;


typedef struct tir_data
{
    short status;
    short frame;
    unsigned cksum;
    float roll, pitch, yaw;
    float tx, ty, tz;
    float padding[9];
} tir_data_t;

typedef struct tir_signature
{
    char DllSignature[200];
    char AppSignature[200];
} tir_signature_t;
