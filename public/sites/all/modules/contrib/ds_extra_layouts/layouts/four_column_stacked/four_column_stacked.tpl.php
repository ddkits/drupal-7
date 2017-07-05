<?php
/**
 * @file
 * Display Suite Four Column Stacked template.
 *
 * Available variables:
 *
 * Layout:
 * - $classes: String of classes that can be used to style this layout.
 * - $contextual_links: Renderable array of contextual links.
 * - $layout_wrapper: wrapper surrounding the layout.
 *
 */

?>
<<?php print $layout_wrapper; print $layout_attributes; ?> class="<?php print $classes;?> clearfix">

  <!-- Needed to activate contextual links. -->
  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php if ($header): ?>
    <<?php print $header_wrapper; ?> class="group-header<?php print $header_classes; ?>">
      <?php print $header; ?>
    </<?php print $header_wrapper; ?>>
  <?php endif; ?>

  <?php if ($first): ?>
    <<?php print $first_wrapper; ?> class="group-first<?php print $first_classes; ?>">
      <?php print $first; ?>
    </<?php print $first_wrapper; ?>>
  <?php endif; ?>

  <?php if ($second): ?>
    <<?php print $second_wrapper; ?> class="group-second<?php print $second_classes; ?>">
      <?php print $second; ?>
    </<?php print $second_wrapper; ?>>
  <?php endif; ?>

  <?php if ($third): ?>
    <<?php print $third_wrapper; ?> class="group-third<?php print $third_classes; ?>">
      <?php print $third; ?>
    </<?php print $third_wrapper; ?>>
  <?php endif; ?>

  <?php if ($fourth): ?>
    <<?php print $fourth_wrapper; ?> class="group-fourth<?php print $fourth_classes; ?>">
      <?php print $fourth; ?>
    </<?php print $fourth_wrapper; ?>>
  <?php endif; ?>


  <?php if ($footer): ?>
    <<?php print $footer_wrapper; ?> class="group-footer<?php print $footer_classes; ?>">
      <?php print $footer; ?>
    </<?php print $footer_wrapper; ?>>
  <?php endif; ?>

</<?php print $layout_wrapper ?>>

<!-- Needed to activate display suite support on forms. -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
