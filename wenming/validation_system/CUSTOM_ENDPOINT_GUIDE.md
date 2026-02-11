# Custom API Endpoint Configuration Guide

## Overview

The validation system supports using custom Claude API endpoints, such as third-party proxy services or relay services. This is useful when:

- Using a regional Claude API service
- Using a proxy service for API access
- Using a custom relay service with additional features
- Testing with a local mock API

## Configuration

### 1. Environment Variables

The system uses two environment variables for API configuration:

- `ANTHROPIC_API_KEY`: Your API key (required)
- `ANTHROPIC_API_BASE`: Custom API endpoint URL (optional)

### 2. Setup Steps

#### Step 1: Copy the example environment file

```bash
cd wenming/validation_system
cp .env.example .env
```

#### Step 2: Edit the .env file

Open `.env` in your text editor and configure:

```bash
# Your API key
ANTHROPIC_API_KEY=your_api_key_here

# Custom endpoint (optional)
ANTHROPIC_API_BASE=https://your-custom-endpoint.com/api
```

#### Step 3: Verify configuration

Run the test suite to verify the configuration:

```bash
uv run python test_system.py
```

## Example Configurations

### Official Anthropic API (Default)

```bash
ANTHROPIC_API_KEY=sk-ant-api03-...
# ANTHROPIC_API_BASE is not set or commented out
```

### Third-Party Proxy Service

```bash
ANTHROPIC_API_KEY=cr_6aa8fefde706ba7780ef902200b7ed3b31eab97b51ee7f43b4465426098162a5
ANTHROPIC_API_BASE=https://crsacc.itssx.com/api
```

### Local Development/Testing

```bash
ANTHROPIC_API_KEY=test_key
ANTHROPIC_API_BASE=http://localhost:8000/api
```

## How It Works

The system automatically detects the custom endpoint configuration:

1. **BaseAgent Initialization**: When each agent is created, it checks for `ANTHROPIC_API_BASE`
2. **Client Configuration**: If set, the Anthropic client is initialized with `base_url` parameter
3. **API Calls**: All API calls are automatically routed to the custom endpoint
4. **Fallback**: If not set, the official Anthropic API is used

### Code Implementation

In `agents/base_agent.py`:

```python
# Initialize Anthropic client with custom endpoint if provided
api_key = os.environ.get("ANTHROPIC_API_KEY")
api_base = os.environ.get("ANTHROPIC_API_BASE")

if api_base:
    # Use custom API endpoint
    self.client = Anthropic(api_key=api_key, base_url=api_base)
else:
    # Use official Anthropic API
    self.client = Anthropic(api_key=api_key)
```

## Security Considerations

### 1. API Key Protection

- **Never commit** `.env` file to git
- The `.gitignore` file excludes `.env` automatically
- Use `.env.example` for documentation only (with placeholder values)

### 2. Custom Endpoint Trust

When using a custom endpoint:

- Ensure the endpoint is from a trusted provider
- Verify the endpoint uses HTTPS (not HTTP)
- Understand the data privacy implications
- Review the service's terms of service

### 3. API Key Scope

Different services may have different API key formats:

- Official Anthropic: `sk-ant-api03-...`
- Custom services: May use different prefixes (e.g., `cr_...`)

## Troubleshooting

### Issue: Connection Error

```
Error: Connection refused
```

**Solution**: Verify the `ANTHROPIC_API_BASE` URL is correct and accessible.

### Issue: Authentication Failed

```
Error: 401 Unauthorized
```

**Solution**: Check that your `ANTHROPIC_API_KEY` is valid for the endpoint you're using.

### Issue: Invalid Response Format

```
Error: Unexpected response format
```

**Solution**: Ensure the custom endpoint is compatible with the Anthropic API format.

### Issue: Model Not Found

```
Error: Model 'claude-opus-4-6' not found
```

**Solution**: Some custom endpoints may use different model names. Check the service documentation and update `config.py` if needed.

## Testing Custom Endpoints

### 1. Quick Test

```bash
uv run python test_system.py
```

This will verify:
- Environment variables are loaded
- API client is initialized
- Basic connectivity works

### 2. Full Validation Test

Run a validation on a small test document:

```bash
uv run python main.py --design-doc test_document.md --round 1
```

This tests:
- API calls work correctly
- Response format is compatible
- All agents can communicate with the endpoint

## Switching Between Endpoints

You can easily switch between official and custom endpoints:

### Switch to Official API

Comment out or remove `ANTHROPIC_API_BASE` in `.env`:

```bash
ANTHROPIC_API_KEY=sk-ant-api03-...
# ANTHROPIC_API_BASE=https://custom-endpoint.com/api
```

### Switch to Custom API

Uncomment and set `ANTHROPIC_API_BASE`:

```bash
ANTHROPIC_API_KEY=your_custom_key
ANTHROPIC_API_BASE=https://custom-endpoint.com/api
```

No code changes needed - just restart the validation system.

## Performance Considerations

Custom endpoints may have different:

- **Latency**: Network distance affects response time
- **Rate Limits**: Different services have different limits
- **Availability**: Uptime may vary by provider
- **Cost**: Pricing models may differ

Monitor your validation runs and adjust expectations accordingly.

## Support

For issues related to:

- **Official Anthropic API**: Contact Anthropic support
- **Custom endpoint**: Contact your service provider
- **Validation system**: Check the README.md and TROUBLESHOOTING.md

## Summary

The validation system's custom endpoint support provides flexibility for different deployment scenarios while maintaining security and ease of use. The configuration is simple, secure, and requires no code changes.

**Key Points:**
- Set `ANTHROPIC_API_BASE` in `.env` for custom endpoints
- Leave unset for official Anthropic API
- Never commit `.env` to version control
- Test configuration before running full validations
