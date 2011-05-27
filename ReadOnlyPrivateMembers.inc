<?php

// $Id$

/**
 * @file
 *
 */
module_load_include('inc', 'php_lib', 'Backtrace');

/**
 * 
 */
class ReadOnlyPrivateMembers {

  /**
   *
   * @var string
   */
  protected $owner;
  /**
   * Call depth.
   * 
   * @var int
   */
  protected $depth;
  /**
   *
   * @var array
   */
  protected $members;

  /**
   *
   * @param array $members 
   * @param array $params
   *   Optional parameters that set what the owning class is.
   */
  public function __construct(array $members, array $params = NULL) {
    $this->owner = isset($params['owner']) ? $params['owner'] : get_caller_class(1);
    $this->depth = isset($params['depth']) ? $params['depth'] : 2;
    $this->members = $members;
  }

  /**
   * Clone this object, deeply.
   */
  public function __clone() {
    $this->members = array_replace_recursive($this->members);
  }
  
  /**
   *
   * @param string $name
   * @return boolean
   */
  public function exists($name) {
    if (array_key_exists($name, $this->members)) {
      return isset($this->members[$name]);
    }
    return FALSE;
  }

  /**
   * Any one can access this member.
   * 
   * @param string $name
   * @return mixed
   */
  public function get($name) {
    if ($this->exists($name)) {
      return $this->members[$name];
    }
    return NULL;
  }

  /**
   * Only objects that share the same class as our owner can modify members.
   * 
   * @param string $name
   * @param mixed $value 
   */
  public function set($name, $value) {
    if ($this->exists($name)) {
      if (get_caller_class($this->depth) == $this->owner) {
        $this->members[$name] = $value;
      }
    }
  }

}