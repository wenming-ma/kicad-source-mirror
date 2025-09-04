
#ifndef POLICY_KEYS_H_
#define POLICY_KEYS_H_

#include <cstdint>

#define POLICY_KEY_DATACOLLECTION "DataCollection"
#define POLICY_KEY_PCM "PluginAndContentManager"
#define POLICY_KEY_REQUESTS_CURL_REVOKE "curl\\SslRevoke"

enum class POLICY_CURL_SSL_REVOKE : std::uint32_t
{
	DEFAULT = 0,
	BEST_EFFORT = 1,
	NONE = 2
};

#endif // POLICY_KEYS_H_