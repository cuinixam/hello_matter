#include "qr_code_utils.h"
#include <esp_log.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>
#include <setup_payload/SetupPayload.h>
#include <app/server/Server.h>
#include <app/server/OnboardingCodesUtil.h>

using namespace ::chip;
using namespace ::chip::Credentials;
using namespace ::chip::DeviceLayer;

static const char *TAG = "QR_CODE_UTILS";

void generate_and_print_qr_code()
{
    // Create buffer for QR code that can fit max size and null terminator.
    char qrCodeBuffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
    chip::MutableCharSpan qrCodeText(qrCodeBuffer);

    // Get QR Code and emulate its content using NFC tag
    GetQRCode(qrCodeText, chip::RendezvousInformationFlags(2));
    // Print QR code to the console
    ESP_LOGI(TAG, "QR Code: %s", qrCodeText.data());
}
