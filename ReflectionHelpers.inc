<?php

// $Id$

/**
 * @file
 * 
 * A collection of functions that help with reflection like properties.
 * 
 */

/**
 * Check to is if the object matches the given type.
 * 
 * @param object $obj
 * @param string $type 
 * 
 * @return boolean
 */
function is_type($obj, $type) {
  return isset($obj) && is_object($obj) && is_string($type) && get_class($obj) == $type;
}

/**
 * Checks if the object is a decendant of the provided type.
 * 
 * @param object $child
 * @param string $type
 * 
 * @return boolean
 */
function is_descendant($obj, $type) {
  $class = isset($obj) && is_object($obj) ? get_class($obj) : FALSE;
  if ($class) {
    $reflection = new ReflectionClass($class);
    return $reflection->isSubclassOf($type);
  }
  return FALSE;
}

/**
 * Checks to see if the object matches the given type, or if the object descends from the given type.
 * 
 * @param object $obj
 * @param string $type 
 * 
 * @return boolean
 */
function is_or_descends_from($obj, $type) {
  return is_type($obj, $type) || is_descendant($obj, $type);
}