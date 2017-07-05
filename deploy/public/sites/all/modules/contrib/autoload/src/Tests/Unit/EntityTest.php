<?php

namespace Drupal\autoload\Tests\Unit;

/**
 * Class EntityTest.
 */
class EntityTest extends AuxiliaryTest {

  /**
   * {@inheritdoc}
   */
  protected static $modules = array('autoload_test_entity_ui');

  /**
   * {@inheritdoc}
   */
  public function test() {
    // The next error must not appear:
    // Error: Class 'Drupal\autoload_test_entity_ui\ViewsController' not found
    // in entity_views_data() (line 31 of sites/all/modules/entity/views/entity.
    // views.inc).
  }

}
