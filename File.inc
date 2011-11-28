<?php

/**
 * @file
 *
 * Helper functions for file processing.
 * 
 * Notes: 
 *  Beware of fseek() you can move it to the end of the file or beyond the end of the file, and feof() won't return the 
 *  correct value as it depends on attempting to 'read' passed the end of the file to work.
 * 
 * 
 * End of line (EOL) sequences
 *  Windows end of line sequence:  \r\n
 *  Unix end of line sequence: \n
 *  Mac end of line sequence: \r
 */

/**
 * Moves the file pointer to the begining of the current line.
 * 
 * If on the first line moves the pointer to 0.
 * 
 * Any other line the pointer will be moved to the character following the previous line's EOL sequence as defined at 
 * in this files header.
 * 
 * The EOL is considered part of that line.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_start($handlem, $read_size = 64) {
  if (fpeekc($handle) === FALSE) { // If at the EOF
    file_end($handle); // Ignore pointless read pass EOF.
  }
  do { // Parse a section of the file looking for an EOL character.
    $pos = ftell($handle);
    $length = min($pos, $read_size);
    $last_section = ($pos - $length) == 0;
    fseek($handle, -$length, SEEK_CUR);
    /**
     * Since we are looking for the character immediately following the EOL it not important if we get both 
     * characters from Windows EOL
     */
    $s = fread($handle, $length); // Get section to parse.
    // Search for the last carriage return in this section
    $pattern = '/\r[^\r]*$/D'; // Last "\r"
    $matches = array();
    preg_match($pattern, $s, $matches, PREG_OFFSET_CAPTURE);
    $carriage_return_pos = isset($matches[0][1]) ? $matches[0][1] : NULL;
    // Search for the last new line in this section
    $pattern = '/\n[^\n]*$/D'; // Last "\n"
    $matches = array();
    preg_match($pattern, $s, $matches, PREG_OFFSET_CAPTURE);
    $new_line_pos = isset($matches[0][1]) ? $matches[0][1] : NULL;
    // Check if we found an EOL character
    $found_at = max($carriage_return_pos, $new_line_pos);
    // Move to the next section.
    fseek($handle, -$length, SEEK_CUR);
  } while (!$found_at && !$last_section);
  if ($found_at) { // Move pointer to character following the EOL
    $offset = -($length - $found_at);
    fseek($handle, $offset, SEEK_CUR);
  }
  return ftell($handle); // Current positon will be the character following the previous lines "\n" or the start of the file
}

/**
 * Moves the file pointer to the end of the current line, or the end of the file if not "\n" is found.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_end($handle) {
  $c = fpeekc($handle);
  if ($c == "\n") { // If Starting at the very end of the current line exit.
    return ftell($handle);
  }
  else if ($c === FALSE) { // If at the EOF, return to the exact EOF and exit.
    file_end($handle);
    return ftell($handle);
  }
  do {
    $c = fgetc($handle);
  } while ($c != "\n" && $c !== FALSE);
  // While we have not reached the end of the current line, or reached the end of the file.
  if ($c == "\n") { // Passed the "\n"
    fseek($handle, -1, SEEK_CUR); // Move the pointer back to "\n"
  }
  return ftell($handle); // Current position will be either "\n" or EOF.
}

/**
 * Moves the file pointer to the begining of the previous line.
 * 
 * This function can wrap the file, if called on the first line of the file the file pointer
 * will be placed at the last line of the file.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_prev($handle) {
  $pos = file_line_start($handle);
  $pos = $pos - 2; // Move back and ignore the current character and the "\n".
  fseek($handle, $pos, SEEK_SET);
  return file_line_start($handle);
}

/**
 * Moves the file pointer to the begining of the previous line.
 * 
 * This function can wrap the file, if called on the last line of the file the file pointer
 * will be placed at the first line of the file.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_next($handle) {
  $pos = file_line_end($handle);
  $pos = feof($handle) ? 0 : $pos + 1; // Return to start or skip the "\n" onto the next line

  fseek($handle, $pos, SEEK_SET); // fseek not appropriate as a 'read' function is not called afterward.
  return ftell($handle);
}

/**
 * Moves the file pointer back the given number of $lines. 
 * 
 * The file pointer will be at the begining of the destination line.
 * 
 * This function won't wrap the file, if the begining of the file is reached the function exits.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * @param int $lines
 *   The max number of lines to move backward in the file.
 * 
 * @return int
 *   The file position
 */
function file_move_back($handle, $lines) {
  $pos = file_line_start($handle);
  if ($pos != 0) {
    for ($i = 0; $i < $lines; $i++) {
      if (($pos = file_line_prev($handle)) == 0) {
        break;
      }
    }
  }
  return ftell($handle);
}

/**
 * Moves the file pointer forward the given number of $lines.
 * 
 * The file pointer will be at the begining of the destination line.
 * 
 * This function won't wrap the file, if the end of the file is reached the function exits.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * @param int $lines
 *   The max number of lines to move forward in the file.
 * 
 * @return int
 *   The file position
 */
function file_move_forward($handle, $lines) {
  $pos = file_line_start($handle);
  for ($i = 0; $i < $lines; $i++) {
    if (($pos = file_line_next($handle)) == 0) {
      file_line_prev($handle);
      break;
    }
  }
  return ftell($handle);
}

/**
 * Similar to fgetcsv, except it ignores enclosures, as fgetcvs breaks with open ended quotes.
 * 
 * Also the number of expected fields can be given in which case the function will read multiple lines 
 * until it has gotten the all the required fields. This helps deal with files that don't have properly 
 * escaped newlines. The newlines will be preserved in the returned values.
 * 
 * Please check too see if fgetcsv works before using this function as its scope is limited, it doesn't deal with 
 * enclosures or escapes correctly.
 * 
 * Note that it is possible for this function to return more fields than expected.
 * 
 * @param resource $handle
 *   File handle.
 * @param int $length
 *   The max number of bytes to read from the current line.
 * @param string $delimiter
 *   A single character used to delimit the fields in the csv.
 * @param int $expected_fields
 *   The number of fields expected to be read for a single line.
 * 
 * @return array
 *   An array containing the values in each field.
 */
function file_get_csv($handle, $length = 0, $delimiter = ',', $expected_fields = NULL) {
  $string = (isset($length) || $length > 0) ? fgets($handle, $length) : fgets($handle); // fget will issue a warning if given 0 or NULL for length.
  if (isset($expected_fields)) {
    $fields = explode($delimiter, $string);
    while (count($fields) < $expected_fields) {
      $string = (isset($length) || $length > 0) ? fgets($handle, $length) : fgets($handle); // fget will issue a warning if given 0 or NULL for length.
      $next_line_fields = explode($delimiter, $string);
      // Merge the last field with the start of this one.
      $fields[count($fields) - 1] .= array_shift($next_line_fields);
      $fields = array_merge($fields, $next_line_fields);
    }
  }
  else {
    $fields = explode($delimiter, $string); // No concern for the number of expected fields.
  }
  // Trim the last new line as it is meant to delimit the end of the row.
  $last_index = count($fields) - 1;
  $fields[$last_index] = trim($fields[$last_index]);
  return $fields;
}

/**
 * Moves the file pointer to the start of the file.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return boolean
 *   TRUE if successful, FALSE otherwise.
 */
function file_start($handle) {
  return fseek($handle, 0, SEEK_SET) == 0;
}

/**
 * Moves the file pointer to the end of the file.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return boolean
 *   TRUE if successful, FALSE otherwise.
 */
function file_end($handle) {
  return fseek($handle, 0, SEEK_END) == 0;
}

/**
 * feof() only works after a 'read' call like fgetc(), so functions like fseek() will invalidated it.
 * 
 * This function will work the same as feof() except it does not require a 'read' call before.
 * 
 * @param resource $handle
 *   File handle.
 */
function file_eof($handle) {
  $pos = ftell($handle);
  $c = fgetc($handle);
  fseek($handle, $pos, SEEK_SET);
  return $c === FALSE;
}

/**
 * Like fgetc() except after getting the current character the file pointer moves backward one instead of forward.
 * 
 * Will not wrap around to the end the file.
 * 
 * Returns FALSE when the character is EOF.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return string
 *   The single character read, or FALSE if EOF.
 */
function fungetc($handle) {
  $c = fpeekc($handle);
  fseek($handle, -1, SEEK_CUR); // Attempt to move to the previous character.
  return $c;
}

/**
 * Peeks at the current character.
 * 
 * Doesn't move the file pointer.
 * 
 * Returns FALSE when the character is EOF.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return string
 *   The single character read, or FALSE if EOF.
 */
function fpeekc($handle) {
  $pos = ftell($handle);
  $c = fgetc($handle);
  fseek($handle, $pos, SEEK_SET);
  return $c;
}

/**
 * Peeks at the current line.
 * 
 * Doesn't move the file pointer.
 * 
 * Reading ends when length - 1 bytes have been read, on a newline (which is included in the return value), 
 * or on EOF (whichever comes first). If no length is specified, it will keep reading from the stream until 
 * it reaches the end of the line.
 * 
 * feof() will not work immeditely after this function is called. Use file_eof() instead.
 * 
 * @param resource $handle
 *   File handle.
 * @param int $length
 *   The max number of bytes to read from the current line.
 * 
 * @return string
 *   The single character read, or FALSE if EOF.
 */
function fpeeks($handle, $length = NULL) {
  $pos = ftell($handle);
  $s = isset($length) ? fgets($handle, $length) : fgets($handle);
  fseek($handle, $pos, SEEK_SET);
  return $s;
}