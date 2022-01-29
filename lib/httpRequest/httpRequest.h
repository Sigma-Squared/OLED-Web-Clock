#include <HTTPClient.h>
#include <ArduinoJSON.h>
#define DEFAULT_RETRY_COUNT 4

String httpGet(HTTPClient &http, const char *url, uint8_t retryCount = DEFAULT_RETRY_COUNT);
String httpGet(HTTPClient &http, String url, uint8_t retryCount = DEFAULT_RETRY_COUNT);
