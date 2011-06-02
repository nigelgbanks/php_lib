<?php

// $Id$

/**
 * @file
 *
 * A collection of functions to help with DOM processing.
 */

/**
 * Gets the a xpath for the given node.
 * 
 * The returned xpath will return the given node only.
 * 
 * @param DOMNode $node
 * @return type 
 */
function get_dom_node_xpath(DOMNode $node) {
  $path = get_dom_node_xpath_fragment($node);
  while (($node = $node->parentNode) && get_class($node) == 'DOMElement') {
    $name = get_dom_node_xpath_fragment($node);
    $path = "$name/$path";
  }
  return "/$path";
}

/**
 * Gets a fragment of the xpath for the given node.
 * 
 * @param DOMNode $node
 * @return type 
 */
function get_dom_node_xpath_fragment(DOMNode $node) {
  if (get_class($node) == 'DOMAttr') {
    return "@{$node->nodeName}";
  }
  else {
    $index = get_dom_node_xpath_index($node->parentNode, $node);
    return "{$node->nodeName}[$index]";
  }
}

/**
 * Gets the child index for the given child element in its parent, 
 * for the purpose of being used to generate an xpath.
 * 
 * @param DOMNode $parent
 * @param DOMNode $child
 * @return int 
 */
function get_dom_node_xpath_index(DOMNode $parent, DOMNode $child) {
  $xpath = new DOMXPath($child->ownerDocument);
  $name = $child->localName;
  $results = $xpath->query("child::*[local-name() = '$name']", $parent);
  $count = $results->length;
  $index = 1;
  for ($i = 0; $i < $count; $i++) {
    $node = $results->item($i);
    if ($child->isSameNode($node)) {
      break;
    }
    $index++;
  }
  return $index;
}

/**
 * Converts a DOMNodeList to an array.
 * 
 * @param DOMNodeList $list
 * @return array
 */
function dom_node_list_to_array(DOMNodeList $list) {
  $count = $list->length;
  $output = array();
  for ($i = 0; $i < $count; $i++) {
    $output[] = $list->item($i);
  }
  return $output;
}

