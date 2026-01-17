
#pragma once

#include <memory>
#include <string>

class QPixmap;

/**
 * Store information to the system clipboard.
 *
 * @param aText is the information to be stored, expected UTF8 encoding.  The text will be
 *              stored as Unicode string (not stored as UTF8 string).
 * @return False if error occurred.
 */
bool SaveClipboard( const std::string& aTextUTF8 );

/**
 * Return the information currently stored in the system clipboard.
 *
 * If data stored in the clipboard is in non-text format, empty string is returned.
 *
 * @note The clipboard is expected containing Unicode chars, not only ASCII7 chars.
 *       The returned string is UTF8 encoded
 */
std::string GetClipboardUTF8();

/**
 * Get image data from the clipboard, if there is any.
 *
 * If there's a filename there, and it can be loaded as an image, do that.
 */
std::unique_ptr<QPixmap> GetImageFromClipboard();
