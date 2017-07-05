<?php

/**
 * @file
 * Based on commerce/modules/customer/commerce_customer.checkout_pane.inc
 */

/**
 * Checkout pane callback: returns the customer profile pane's settings form.
 */
function commerce_addressbook_pane_settings_form($checkout_pane) {
  $form = array();

  // Extract the type of profile represented by this pane from its ID.
  $type = substr($checkout_pane['pane_id'], 17); // Removes 'customer_profile_'

  // Build an options array of customer profile reference fields available for
  // the data from this customer profile pane.
  $options = array();

  foreach (commerce_info_fields('commerce_customer_profile_reference', 'commerce_order') as $name => $field) {
    if ($type == $field['settings']['profile_type']) {
      $instance = field_info_instance('commerce_order', $name, 'commerce_order');
      $options[$name] = check_plain($instance['label']);
    }
  }

  $form['commerce_' . $checkout_pane['pane_id'] . '_field'] = array(
    '#type' => 'select',
    '#title' => t('Related customer profile reference field'),
    '#description' => t('Specify the customer profile reference field on the order to populate with profile data from this checkout pane.'),
    '#options' => $options,
    '#empty_value' => '',
    '#default_value' => variable_get('commerce_' . $checkout_pane['pane_id'] . '_field', ''),
    '#required' => TRUE,
  );

  // Provide the option to copy values from other profile types if they exist.
  $profile_types = commerce_customer_profile_type_options_list();
  unset($profile_types[$type]);

  if (count($profile_types)) {
    $form['commerce_' . $checkout_pane['pane_id'] . '_profile_copy'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable profile copying on this checkout pane, helping customers avoid having to enter the same address twice.'),
      '#default_value' => variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy', FALSE),
    );

    $form['commerce_' . $checkout_pane['pane_id'] . '_profile_copy_wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Profile copy options'),
      '#states' => array(
        'visible' => array(
          ':input[name="commerce_' . $checkout_pane['pane_id'] . '_profile_copy"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['commerce_' . $checkout_pane['pane_id'] . '_profile_copy_wrapper']['commerce_' . $checkout_pane['pane_id'] . '_profile_copy_source'] = array(
      '#type' => 'select',
      '#title' => t('Profile to copy from'),
      '#options' => $profile_types,
      '#default_value' => variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy_source', NULL),
    );

    $form['commerce_' . $checkout_pane['pane_id'] . '_profile_copy_wrapper']['commerce_' . $checkout_pane['pane_id'] . '_profile_copy_default'] = array(
      '#type' => 'checkbox',
      '#title' => t('Make copying information from this profile the default action, requiring users to uncheck a box on the checkout pane to enter a different address.'),
      '#default_value' => variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy_default', TRUE),
    );
  }

  return $form;
}

/**
 * Checkout pane callback: returns a customer profile edit form.
 */
function commerce_addressbook_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  global $user;
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['pane'] = drupal_get_path('module', 'commerce_addressbook') . '/commerce_addressbook.checkout_pane.inc';

  $pane_id = $checkout_pane['pane_id'];
  // Extract the type of profile represented by this pane from its ID.
  $type = substr($pane_id, 17); // Removes 'customer_profile_'

  $field_name = variable_get('commerce_' . $pane_id . '_field', '');
  // The specified profile reference field doesn't exist on this order type.
  // A consequence of the checkout settings applying for all order types.
  if ($field_name && !field_info_instance('commerce_order', $field_name, $order->type)) {
    return array();
  }

  // Find the referenced profile using the related reference field...
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $profile = NULL;
  $mode = 'view';

  // If the associated order field has been set...
  if ($field_name) {
    $profile = $wrapper->{$field_name}->value();
  }
  else {
    // Or try the association stored in the order's data array if no field is set.
    if (!empty($order->data['profiles'][$checkout_pane['pane_id']])) {
      $profile = commerce_customer_profile_load($order->data['profiles'][$pane_id]);
    }
  }

  // If an AJAX rebuild happened, we might have our data in form state
  if (!empty($form_state['pane_' . $pane_id])) {
    $profile = $form_state['pane_' . $pane_id]['profile'];
    $mode = $form_state['pane_' . $pane_id]['mode'];
  }

  $valid_profiles = array();
  $incomplete_profiles = array();
  // Load the active user profiles.
  if ($user->uid && $active_profiles = commerce_addressbook_get_active_profiles($user->uid, $type)) {
    foreach ($active_profiles as $active_profile) {
      if (commerce_addressbook_profile_is_incomplete($active_profile)) {
        $incomplete_profiles[$active_profile->profile_id] = $active_profile;
      }
      else {
        $valid_profiles[$active_profile->profile_id] = $active_profile;
      }
    }
  }

  // No profile set yet. First check if one exists already.
  if (empty($profile)) {
    if (!$valid_profiles) {
      // Incomplete profiles need to go into edit mode.
      if ($incomplete_profiles) {
        $mode = 'edit';
        $profile = reset($incomplete_profiles);
      }
      else {
        // No profiles found. Create a new one.
        $profile = commerce_customer_profile_new($type, $order->uid);
        $mode = 'new';
      }
    }
    else {
      $default_profile_id = commerce_addressbook_get_default_profile_id($user->uid, $type);

      // Try to see if the default profile is in the valid profiles array.
      if (isset($valid_profiles[$default_profile_id])) {
        $profile = $valid_profiles[$default_profile_id];
      }
      else {
        // Otherwise, just select the first one.
        $profile = reset($valid_profiles);
      }
    }
  }
  $ajax_wrapper = strtr($pane_id, '_', '-') . '-ajax-wrapper';
  $pane_form = array(
    '#parents' => array($pane_id),
    '#prefix' => '<div id="' . $ajax_wrapper . '">',
    '#suffix' => '</div>',
  );
  _commerce_addressbook_add_profile_copy_checkbox($pane_form, $checkout_pane, $order, $type);

  $profile_copy_enabled = FALSE;
  // If profile copying is enabled.
  if (isset($pane_form['commerce_customer_profile_copy']) && !empty($pane_form['commerce_customer_profile_copy']['#default_value'])) {
    $profile_copy_enabled = TRUE;
    $mode = 'copy';
    field_attach_form('commerce_customer_profile', $profile, $pane_form, $form_state);
    $source_profile_type_name = variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy_source', NULL);
    if ($source_profile_type = commerce_customer_profile_type_load($source_profile_type_name)) {
      $common_fields = array();

      foreach (field_info_instances('commerce_customer_profile', $source_profile_type['type']) as $field_name => $field) {
        // If the field exists on the destination profile then disable it.
        if (!empty($pane_form[$field_name])) {
          $langcode = $pane_form[$field_name]['#language'];
          $pane_form[$field_name][$langcode]['#access'] = FALSE;
          $common_fields[] = $field_name;
        }
      }

      // Look for a matching destination profile in the valid profiles array.
      $source_pane_id = 'pane_customer_profile_' . $source_profile_type_name;
      // Check if we have multiple valid profiles, verify that the source
      // pane is in the "view" mode.
      if (count($valid_profiles) > 1 && $common_fields && isset($form_state[$source_pane_id]) && $form_state[$source_pane_id]['mode'] == 'view') {
        $source_profile = $form_state[$source_pane_id]['profile'];

        // Loop over the valid profiles to find a matching one.
        foreach ($valid_profiles as $valid_profile) {
          // Compare the fields that are common to both profile types.
          foreach ($common_fields as $field_name) {
            if ($source_profile->{$field_name} !== $valid_profile->{$field_name}) {
              continue 2;
            }
          }

          // Update the customer profile used by this pane to use the matching
          // one.
          $profile = $valid_profile;
          break;
        }
      }
    }
  }

  // Force the edit mode if we couldn't find "valid" profiles.
  if ($mode == 'view' && !empty($profile->profile_id) && !$valid_profiles) {
    $mode = 'edit';
  }

  // Remember the current profile and mode in form state.
  $form_state['pane_' . $checkout_pane['pane_id']] = array(
    'profile' => $profile,
    'mode' => $mode,
  );

  // Add the entity context of the current cart order.
  $profile->entity_context = array(
    'entity_type' => 'commerce_order',
    'entity_id' => $order->order_id,
  );

  // When the profile copy is checked, don't show any other form elements.
  if ($profile_copy_enabled) {
    return $pane_form;
  }

  if ($mode == 'view') {
    $content = entity_view('commerce_customer_profile', array($profile->profile_id => $profile), 'customer');

      // Prepare the options.
    $options = array();
    foreach ($valid_profiles as $id => $customer_profile) {
      $field_values = field_get_items('commerce_customer_profile', $customer_profile, 'commerce_customer_address');
      $options[$customer_profile->profile_id] = $field_values[0]['thoroughfare'];
    }
    $options['new_address'] = t('+ Enter a new address');
    drupal_alter('commerce_addressbook_labels', $options, $valid_profiles);

    $pane_form['profile_selection'] = array(
      '#title' => t('Select an address'),
      '#options' => $options,
      '#type' => 'select',
      '#ajax' => array(
        'callback' => 'commerce_addressbook_pane_refresh',
        'wrapper' => $ajax_wrapper,
      ),
      '#element_validate' => array('commerce_addressbook_saved_addresses_validate'),
      '#default_value' => $profile->profile_id,
    );

    $pane_form['rendered_profile'] = array(
      '#markup' => drupal_render($content),
    );
    $pane_form['edit_button'] = array(
      '#type' => 'submit',
      '#name' => 'pane-' . $pane_id . '-edit',
      '#value' => t('Edit'),
      '#limit_validation_errors' => array(),
      '#ajax' => array(
        'callback' => 'commerce_addressbook_pane_refresh',
        'wrapper' => $ajax_wrapper,
      ),
      '#submit' => array('commerce_addressbook_pane_edit'),
    );
  }
  else {
    // Add the field widgets for the profile.
    field_attach_form('commerce_customer_profile', $profile, $pane_form, $form_state);

    // Tweak the form to remove the fieldset from the address field if there
    // is only one on this profile.
    $addressfields = array();
    foreach (commerce_info_fields('addressfield', 'commerce_customer_profile') as $field_name => $field) {
      if (!empty($pane_form[$field_name]['#language'])) {
        $langcode = $pane_form[$field_name]['#language'];

        // Only consider this addressfield if it's represented on the form.
        if (!empty($pane_form[$field_name][$langcode])) {
          $addressfields[] = array($field_name, $langcode);
        }
      }
    }

    // Check to ensure only one addressfield was found on the form.
    if (count($addressfields) == 1) {
      list($field_name, $langcode) = array_shift($addressfields);

      foreach (element_children($pane_form[$field_name][$langcode]) as $delta) {
        // Don't mess with the "Add another item" button that could be present.
        if ($pane_form[$field_name][$langcode][$delta]['#type'] != 'submit') {
          $pane_form[$field_name][$langcode][$delta]['#type'] = 'container';
        }
      }
    }

    if ($valid_profiles) {
      $pane_form['cancel_button'] = array(
        '#type' => 'submit',
        '#name' => 'pane-' . $pane_id . '-cancel',
        '#value' => t('Return to address selection'),
        '#limit_validation_errors' => array(),
        '#ajax' => array(
          'callback' => 'commerce_addressbook_pane_refresh',
          'wrapper' => $ajax_wrapper,
        ),
        '#submit' => array('commerce_addressbook_pane_cancel'),
      );
    }
 }

  return $pane_form;
}

function _commerce_addressbook_add_profile_copy_checkbox(&$pane_form, $checkout_pane, $order, $target_type) {
  if (variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy', FALSE)
    && $source_profile_type_name = variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy_source', NULL)) {
    // Load the default profile copy option from settings.
    $profile_copy_default = variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy_default', FALSE);

    // Make sure our profile type still exists..
    if ($source_profile_type = commerce_customer_profile_type_load($source_profile_type_name)) {
      $target_profile_type = commerce_customer_profile_type_load($target_type);

      $pane_form['commerce_customer_profile_copy'] = array(
        '#type' => 'checkbox',
        '#title' => t('My %target is the same as my %source.', array(
          '%target' => $target_profile_type['name'],
          '%source' => $source_profile_type['name']
        )),
        '#element_validate' => array('commerce_addressbook_profile_copy_validate'),
        '#default_value' => isset($order->data['profile_copy'][$checkout_pane['pane_id']]['status']) ? $order->data['profile_copy'][$checkout_pane['pane_id']]['status'] : $profile_copy_default,
        '#weight' => -30,
        '#ajax' => array(
          'callback' => 'commerce_addressbook_pane_refresh',
          'wrapper' => strtr($checkout_pane['pane_id'], '_', '-') . '-ajax-wrapper',
        ),
        '#attached' => array(
          'css' => array(drupal_get_path('module', 'commerce_customer') . '/theme/commerce_customer.theme.css'),
        ),
        '#prefix' => '<div class="commerce-customer-profile-copy">',
        '#suffix' => '</div>',
      );
    }
  }
}

/**
 * Element validate callback: processes input of the profile selection list.
 */
function commerce_addressbook_saved_addresses_validate($element, &$form_state, $form) {
  if (in_array('profile_selection', $form_state['triggering_element']['#parents']) && $form_state['triggering_element']['#id'] == $element['#id']) {
    $pane_id = $element['#parents'][0];

    // If the user wants to enter a new address.
    if ($element['#value'] == 'new_address') {
      global $user;
      $form_state['pane_' . $pane_id]['mode'] = 'edit';
      $type = substr($pane_id, 17);
      $form_state['pane_' . $pane_id]['profile'] = commerce_customer_profile_new($type, $user->uid);
    }
    else {
      $form_state['pane_' . $pane_id]['mode'] = 'view';
      $form_state['pane_' . $pane_id]['profile'] = commerce_customer_profile_load($element['#value']);
    }
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Element validate callback for the profile copy checkbox.
 */
function commerce_addressbook_profile_copy_validate($element, &$form_state, $form) {
  $triggering_element = end($form_state['triggering_element']['#array_parents']);
  $pane_id = reset($element['#array_parents']);
  $triggered_by_copy = ($triggering_element == 'commerce_customer_profile_copy' && $form_state['triggering_element']['#id'] == $element['#id']);

  if ($triggering_element == 'commerce_customer_profile_copy') {
    $form_state['rebuild'] = TRUE;
  }

  // Checkbox: Off - Only invoked for the corresponding trigger element.
  if ($triggered_by_copy && empty($element['#value'])) {
    $form_state['order']->data['profile_copy'][$pane_id]['status'] = FALSE;
    // Make sure we return to the "view" mode if possible.
    $form_state['pane_' . $pane_id] = array();
    unset($form_state['order']->data['profile_copy'][$pane_id]['elements']);
    commerce_order_save($form_state['order']);
  }
  // Checkbox: On - Only invoked for the corresponding trigger element, or the
  // "continue" checkout form button.
  elseif (($triggered_by_copy || isset($form_state['triggering_element']['#button_type'])) && !empty($element['#value'])) {
    $type = substr($pane_id, 17);  // Removes 'customer_profile_'
    $source_id = 'customer_profile_' . variable_get('commerce_' . $pane_id . '_profile_copy_source', '');
    $info = array('commerce_customer_profile', $type, $pane_id);

    if (isset($form_state['values'][$source_id]) && !isset($form_state['values'][$source_id]['profile_selection'])) {
      commerce_customer_profile_copy_fields($info, $form_state['input'][$pane_id], $form_state['input'][$source_id], $form_state);
      commerce_customer_profile_copy_fields($info, $form_state['values'][$pane_id], $form_state['values'][$source_id], $form_state);
    }
    else {
      // Use the profile from the source pane.
      $profile = $form_state['pane_' . $source_id]['profile'];

      if (!empty($profile)) {
        commerce_customer_profile_copy_fields($info, $form_state['input'][$pane_id], $profile, $form_state);
        commerce_customer_profile_copy_fields($info, $form_state['values'][$pane_id], $profile, $form_state);
      }
    }

    $form_state['order']->data['profile_copy'][$pane_id]['status'] = TRUE;
    commerce_order_save($form_state['order']);

    // Unset any cached addressfield data for this customer profile.
    if (!empty($form_state['addressfield'])) {
      foreach ($form_state['addressfield'] as $key => $value) {
        if (strpos($key, 'commerce_customer_profile|' . $type) === 0) {
          unset($form_state['addressfield'][$key]);
        }
      }
    }
  }
}

/**
 * Ajax callback for the "edit" button.
 */
function commerce_addressbook_pane_refresh($form, &$form_state) {
  $pane_id = reset($form_state['triggering_element']['#parents']);
  return $form[$pane_id];
}

/**
 * Submit callback for the "edit" button.
 */
function commerce_addressbook_pane_edit($form, &$form_state) {
  $pane_id = reset($form_state['triggering_element']['#parents']);
  $form_state['pane_' . $pane_id]['mode'] = 'edit';
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit callback for the "cancel" button.
 */
function commerce_addressbook_pane_cancel($form, &$form_state) {
  $pane_id = reset($form_state['triggering_element']['#parents']);
  $form_state['pane_' . $pane_id] = array();
  $form_state['rebuild'] = TRUE;
}

/**
 * Checkout pane callback: validates a customer profile edit form.
 */
function commerce_addressbook_pane_checkout_form_validate($form, &$form_state, $checkout_pane, $order) {
  $pane_id = $checkout_pane['pane_id'];
  $field_name = variable_get('commerce_' . $pane_id . '_field', '');
  // The specified profile reference field doesn't exist on this order type.
  // A consequence of the checkout settings applying for all order types.
  if ($field_name && !field_info_instance('commerce_order', $field_name, $order->type)) {
    return TRUE;
  }
  // Skip validation if the profile can't be found.
  if (!isset($form_state['pane_' . $pane_id]['profile']) || !isset($form_state['pane_' . $pane_id]['mode'])) {
    return TRUE;
  }

  $mode = $form_state['pane_' . $pane_id]['mode'];
  $source_profile_type = variable_get('commerce_' . $checkout_pane['pane_id'] . '_profile_copy_source', NULL);

  // Build the source profile pane identifier in order to check for
  // possible validation errors.
  if (!empty($source_profile_type)) {
    $source_id = 'customer_profile_' . $source_profile_type;
  }

  // Validate "edit" and "new" modes.
  if ($mode != 'view') {
    $profile = $form_state['pane_' . $pane_id]['profile'];

    // Notify field widgets to validate their data.
    field_attach_form_validate('commerce_customer_profile', $profile, $form[$checkout_pane['pane_id']], $form_state);

    // If any errors occurred in the field attach validate process for fields on
    // this checkout pane's customer profile, fail the checkout pane validation.
    if ($errors = form_get_errors()) {
      foreach ($errors as $field => $error) {
        if (substr($field, 0, strlen($pane_id) + 2) == $pane_id . '][') {
          return FALSE;
        }
        // If we're copying a profile, check if there are no errors in its pane
        // otherwise, a profile with missing information is going to be saved.
        elseif ($mode == 'copy' && !empty($source_id)) {
          if (substr($field, 0, strlen($source_id) + 2) == $source_id. '][') {
            return FALSE;
          }
        }
      }
    }
  }

  return TRUE;
}

/**
 * Checkout pane callback: submits a customer profile edit form.
 */
function commerce_addressbook_pane_checkout_form_submit($form, &$form_state, $checkout_pane, $order) {
  $pane_id = $checkout_pane['pane_id'];
  $field_name = variable_get('commerce_' . $pane_id . '_field', '');
  // The specified profile reference field doesn't exist on this order type.
  // A consequence of the checkout settings applying for all order types.
  if ($field_name && !field_info_instance('commerce_order', $field_name, $order->type)) {
    return;
  }
  // Skip validation if the profile can't be found.
  if (!isset($form_state['pane_' . $pane_id]['profile']) || !isset($form_state['pane_' . $pane_id]['mode'])) {
    return;
  }

  $profile = $form_state['pane_' . $pane_id]['profile'];
  // Save the customer profile in the "new" and "edit" modes.
  if ($form_state['pane_' . $pane_id]['mode'] != 'view') {
    // Ensure the profile is active.
    $profile->status = TRUE;

    // Set the profile's uid if it's being created at this time.
    if (empty($profile->profile_id)) {
      $profile->uid = $order->uid;
    }

    // Notify field widgets.
    field_attach_submit('commerce_customer_profile', $profile, $form[$checkout_pane['pane_id']], $form_state);

    // Make sure that the incomplete flag is removed, if it was previously set.
    if (!empty($profile->data['incomplete'])) {
      unset($profile->data['incomplete']);
    }

    // Save the profile.
    commerce_customer_profile_save($profile);
  }

  // Store the profile ID for the related field as specified on the settings form.
  $wrapper = entity_metadata_wrapper('commerce_order', $order);

  if ($field_name) {
    $wrapper->{$field_name} = $profile;
  }
  else {
    // Or make the association in the order's data array if no field was found.
    $order->data['profiles'][$checkout_pane['pane_id']] = $profile->profile_id;
  }
}

/**
 * Checkout pane callback: returns the cart contents review data for the
 *   Review checkout pane.
 *
 * Copied verbatim from commerce_customer_profile_pane_review.
 */
function commerce_addressbook_pane_review($form, $form_state, $checkout_pane, $order) {
  // Load the profile based on the related customer profile reference field...
  if ($field_name = variable_get('commerce_' . $checkout_pane['pane_id'] . '_field', '')) {
    $profile = entity_metadata_wrapper('commerce_order', $order)->{$field_name}->value();
  }
  else {
    // Or use the association stored in the order's data array if no field is set.
    $profile = commerce_customer_profile_load($order->data['profiles'][$checkout_pane['pane_id']]);
  }

  if ($profile) {
    $content = entity_view('commerce_customer_profile', array($profile->profile_id => $profile), 'customer');

    return drupal_render($content);
  }
  else {
    return t('No information');
  }
}
