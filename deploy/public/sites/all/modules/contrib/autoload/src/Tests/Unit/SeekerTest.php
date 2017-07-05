<?php

namespace Drupal\autoload\Tests\Unit;

/**
 * Class SeekerTest.
 */
class SeekerTest extends AuxiliaryTest {

  /**
   * {@inheritdoc}
   */
  protected static $modules = array('autoload_test_custom', 'autoload_test_drupal');

  /**
   * {@inheritdoc}
   */
  public static function getInfo() {
    $info = parent::getInfo();
    $info['name'] = 'Seeker';
    $info['description'] = t('Testing class seeker.');

    return $info;
  }

  /**
   * {@inheritdoc}
   */
  public function test() {
    $this->assertClasses('Autoload', array(
      'Autoload\Tests\PSR0',
      'Autoload\Tests\PSR4',
      'Autoload\Tests\Example\Test',
    ));

    $this->assertClasses('AutoloadTests', array(
      'AutoloadTests\PSR4',
    ));

    $this->assertClasses('Drupal', array(
      'Drupal\autoload_test_drupal\PSR0',
      'Drupal\autoload_test_drupal\PSR4',
    ));

    foreach (autoload_seek_classes('Drupal', NULL, TRUE) as $class => $info) {
      $keys = array(
        'uri',
        'name',
        'path',
        'filename',
        'module_name',
        'module_path',
      );

      $this->assertFalse(array_diff(array_keys((array) $info), $keys), 'Structure of file info is correct.');
      // No need to iterate through all of the classes.
      break;
    }
  }

}
