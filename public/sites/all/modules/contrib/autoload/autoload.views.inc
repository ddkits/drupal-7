<?php

/**
 * @file
 * Views integration.
 */

/**
 * Implements hook_views_data().
 */
function autoload_views_data() {
  autoload_paths_recompute();

  return array();
}
