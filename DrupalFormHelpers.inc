<?php

// $Id$

/**
 * @file
 *
 */
module_load_include('inc', 'php_lib', 'Array');

/**
 *
 * @param array $element
 * @param array $form 
 */
function get_form_element_parent(array $element, array &$form) {
  array_pop($element['#array_parents']);
  $parent = &array_traverse_path($form, $element['#array_parents']);
  return $parent ? $parent : NULL;
}