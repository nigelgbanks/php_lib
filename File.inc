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
 * Returns FALSE when the character is EOF.
 * 
 * @param resource $handle
 *   File handle.
 * @return string
 *   The single character read, or FALSE if EOF.
 */
function fpeeks($handle) {
  $pos = ftell($handle);
  $string = fgets($handle);
  fseek($handle, $pos, SEEK_SET);
  return $string;
}