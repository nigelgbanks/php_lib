<?php

// $Id$

/**
 * @file
 *
 */
module_load_include('inc', 'php_lib', 'ReadOnlyPrivateMembers');
module_load_include('inc', 'php_lib', 'ReadOnlyProtectedMembers');

/**
 * 
 */
class ReadOnlyMembers {

  /**
   * Private members that are read only.
   * 
   * @var ReadOnlyPrivateMembers
   */
  protected $private;
  /**
   * Protected members that are read only.
   * 
   * @var ReadOnlyProtectedMembers
   */
  protected $protected;

  /**
   *
   * @param array $private
   * @param array $protected 
   */
  public function __construct(array $private, array $protected) {
    $owner_class = get_caller_class(1);
    $properties = array('owner_class' => $owner_class, 'depth' => 3);
    $this->private = new ReadOnlyPrivateMembers($private, $properties);
    $this->protected = new ReadOnlyProtectedMembers($owner_class, $properties);
  }

  /**
   *
   * @param string $name
   */
  public function exists($name) {
    return $this->private->exists($name) || $this->protected->exists($name);
  }

  /**
   *
   * @param string $name 
   */
  public function get($name) {
    if ($this->private->exists($name)) {
      return $this->private->get($name);
    }
    else if ($this->protected->exists($name)) {
      return $this->protected->get($name);
    }
    return NULL;
  }

  /**
   *
   * @param string $name
   * @param mixed $value 
   */
  public function set($name, $value) {
    if ($this->private->exists($name)) {
      return $this->private->set($name, $value);
    }
    else if ($this->protected->exists($name)) {
      return $this->protected->set($name, $value);
    }
  }

}