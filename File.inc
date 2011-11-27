<?php

/**
 * @file
 *
 * Helper functions for file processing.
 */

/**
 * Moves the file pointer to the begining of the current line.
 * 
 * If on the first line moves the pointer to 0.
 * 
 * Any other line the pointer will be moved to the character following the previous line's "\n"
 * 
 * The "\n" at the end of a line is considered part of that line.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_start($handle) {
  $pos = ftell($handle);
  $pos = (fpeekc($handle) == "\n") ? $pos - 1 : $pos; // Starting at the very end of the current line, ignore the "\n"
  do {
    fseek($handle, $pos, SEEK_SET);
    $c = fgetc($handle);
    $pos = $pos - 1;
  } while ($c != "\n" && $pos != -2);
  // While we have not reached the end of the previous line 
  // or passed the begining of the file.
  if (feof($handle)) { // Wrapped the document set back to start.
    fseek($handle, 0, SEEK_SET);
  }
  return ftell($handle); // Current positon will be the character following the previous lines "/n" or the start of the file
}

/**
 * Moves the file pointer to the end of the current line, or the end of the file if not "/n" is found.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_end($handle) {
  $pos = ftell($handle);
  do {
    fseek($handle, $pos, SEEK_SET);
    $c = fpeekc($handle);
    $pos = $pos + 1;
  } while ($c != "\n" && !feof($handle));
  // While we have not reached the end of the current line
  // or the end of the file.
  return ftell($handle); // Current position will be either "\n" or EOF.
}

/**
 * Moves the file pointer to the begining of the previous line.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_prev($handle) {
  $pos = file_line_start($handle);
  $pos = $pos - 2; // Move back and ignore the current character and the "/n".
  fseek($handle, $pos, SEEK_SET);
  return file_line_start($handle);
}

/**
 * Moves the file pointer to the begining of the previous line.
 * 
 * @param resource $handle
 *   File handle.
 * 
 * @return int
 *   The file position
 */
function file_line_next($handle) {
  $pos = file_line_end($handle);
  $pos = $pos + 2; // Move back and ignore the current character and the "/n".
  fseek($handle, $pos, SEEK_SET);
  do {
    fseek($handle, $pos, SEEK_END);
    $t = fgetc($handle);
    $pos = $pos - 1;
  } while ($t != "\n");
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
 * Peeks at the current character.
 * 
 * Doesn't move the file pointer.
 * 
 * Returns FALSE when the character is EOF.
 * 
 * @param resource $handle
 *   File handle.
 * @return string
 *   The single character read, or FALSE if EOF.
 */
function fpeekc($handle) {
  $pos = ftell($handle);
  $char = fgetc($handle);
  fseek($handle, $pos, SEEK_SET);
  return $char;
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
  $string = isset($length) ? fgets($handle, $length) : fgets($handle);
  fseek($handle, $pos, SEEK_SET);
  return $string;
}