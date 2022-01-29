#include "httpRequest.h"

String httpGet(HTTPClient &http, const char *url, uint8_t retryCount)
{
    http.begin(url);

    while (retryCount--)
    {
        int responseCode = http.GET();
        if (responseCode >= 200 && responseCode < 300)
        {
            break;
        }
        else
        {
            Serial.printf("[%i/%i] %i on GET request: to %s\n", 4 - retryCount, DEFAULT_RETRY_COUNT, responseCode, url);
        }
    }
    String payload = "";
    payload = http.getString();
    http.end();
    return payload;
}

String httpGet(HTTPClient &http, String url, uint8_t retryCount)
{
    return httpGet(http, url.c_str(), retryCount);
}