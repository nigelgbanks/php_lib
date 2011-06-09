<?php

// $Id$

/**
 * @file
 *
 * A collection of functions to help with DOM processing.
 */
module_load_include('inc', 'php_lib', 'String');

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

/**
 * Formats and indents the given DOMDocument.
 * 
 * @param DOMDocument $document 
 */
function format_dom_document(DOMDocument $document) {
  $filename = drupal_get_path('module', 'php_lib') . '/js/pretty-print/PrettyPrint.xsl';
  $xsl = new DOMDocument();
  $xsl->load($filename);
  $proc = new XSLTProcessor();
  $proc->importStylesheet($xsl);
  $xml = $proc->transformToXml($document);
  $document->loadXML($xml);
}

/**
 * Pretty print the given document, to the browswer.
 * 
 * @param DOMDocument $document 
 */
function dom_document_pretty_print(DOMDocument $document) {
  format_dom_document($document);
  $content = htmlentities($document->saveXML());
  $content = append_to_new_line($content, '<br/>');
  $path = drupal_get_path('module', 'php_lib');
  echo("<script type='text/javascript' src='$path/js/pretty-print/prettify.js'></script>");
  echo("<link type='text/css' rel='stylesheet' media='all' href='$path/js/pretty-print/prettify.css'>");
  echo("<script type='text/javascript' src='$path/js/pretty-print/prettify.js'></script>");
  echo("<code class='prettyprint xml' style='overflow: scroll; position: relative;'>$content</code>");
  echo("<script type='text/javascript'>prettyPrint();</script>");
}