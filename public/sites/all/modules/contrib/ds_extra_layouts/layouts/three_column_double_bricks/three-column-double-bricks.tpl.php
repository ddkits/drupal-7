<?php
/**
 * @file
 * Display Suite Three Column Bricks template.
 *
 * Available variables:
 *
 * Layout:
 * - $classes: String of classes that can be used to style this layout.
 * - $contextual_links: Renderable array of contextual links.
 * - $layout_wrapper: wrapper surrounding the layout.
 *
 * Regions:
 *
 * - $header: Rendered content for the "Header" region.
 * - $header_classes: String of classes that can be used to style the "Header" region.
 *
 * - $above_left: Rendered content for the "Above Left" region.
 * - $above_left_classes: String of classes that can be used to style the "Above Left" region.
 *
 * - $above_middle: Rendered content for the "Above Middle" region.
 * - $above_middle_classes: String of classes that can be used to style the "Above Middle" region.
 *
 * - $above_right: Rendered content for the "Above Right" region.
 * - $above_right_classes: String of classes that can be used to style the "Above Right" region.
 *
 * - $above_brick: Rendered content for the "Above Brick" region.
 * - $above_brick_classes: String of classes that can be used to style the "Above Brick" region.
 *
 * - $middle_left: Rendered content for the "Middle Left" region.
 * - $middle_left_classes: String of classes that can be used to style the "Middle Left" region.
 *
 * - $middle_middle: Rendered content for the "Middle Middle" region.
 * - $middle_middle_classes: String of classes that can be used to style the "Middle Middle" region.
 *
 * - $middle_right: Rendered content for the "middle Right" region.
 * - $middle_right_classes: String of classes that can be used to style the "Middle Right" region.
 *
 * - $below_brick: Rendered content for the "Below Brick" region.
 * - $below_brick_classes: String of classes that can be used to style the "Below Brick" region.
 *
 * - $below_left: Rendered content for the "Below Left" region.
 * - $below_left_classes: String of classes that can be used to style the "Below Left" region.
 *
 * - $below_middle: Rendered content for the "Below Middle" region.
 * - $below_middle_classes: String of classes that can be used to style the "Below Middle" region.
 *
 * - $below_right: Rendered content for the "Below Right" region.
 * - $below_right_classes: String of classes that can be used to style the "Below Right" region.
 *
 * - $footer: Rendered content for the "Footer" region.
 * - $footer_classes: String of classes that can be used to style the "Footer" region.
 */
?>
<<?php print $layout_wrapper; print $layout_attributes; ?> class="<?php print $classes;?> clearfix">

  <!-- Needed to activate contextual links. -->
  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

    <<?php print $header_wrapper; ?> class="ds-header<?php print $header_classes; ?>">
      <?php print $header; ?>
    </<?php print $header_wrapper; ?>>

    <div class="ds-above">
    <<?php print $above_left_wrapper; ?> class="ds-above-left<?php print $above_left_classes; ?>">
      <?php print $above_left; ?>
    </<?php print $above_left_wrapper; ?>>

    <<?php print $above_middle_wrapper; ?> class="ds-above-middle<?php print $above_middle_classes; ?>">
      <?php print $above_middle; ?>
    </<?php print $above_middle_wrapper; ?>>

    <<?php print $above_right_wrapper; ?> class="ds-above-right<?php print $above_right_classes; ?>">
      <?php print $above_right; ?>
    </<?php print $above_right_wrapper; ?>>
    </div>

    <<?php print $above_brick_wrapper; ?> class="ds-above-brick<?php print $above_brick_classes; ?>">
      <?php print $above_brick; ?>
    </<?php print $above_brick_wrapper; ?>>

    <div class="ds-middle">
    <<?php print $middle_left_wrapper; ?> class="ds-middle-left<?php print $middle_left_classes; ?>">
      <?php print $middle_left; ?>
    </<?php print $middle_left_wrapper; ?>>

    <<?php print $middle_middle_wrapper; ?> class="ds-middle-middle<?php print $middle_middle_classes; ?>">
      <?php print $middle_middle; ?>
    </<?php print $middle_middle_wrapper; ?>>

    <<?php print $middle_right_wrapper; ?> class="ds-middle-right<?php print $middle_right_classes; ?>">
      <?php print $middle_right; ?>
    </<?php print $middle_right_wrapper; ?>>
    </div>

    <<?php print $below_brick_wrapper; ?> class="ds-below-brick<?php print $below_brick_classes; ?>">
      <?php print $below_brick; ?>
    </<?php print $below_brick_wrapper; ?>>

    <div class="ds-below">
    <<?php print $below_left_wrapper; ?> class="ds-below-left<?php print $below_left_classes; ?>">
      <?php print $below_left; ?>
    </<?php print $below_left_wrapper; ?>>

    <<?php print $below_middle_wrapper; ?> class="ds-below-middle<?php print $below_middle_classes; ?>">
      <?php print $below_middle; ?>
    </<?php print $below_middle_wrapper; ?>>

    <<?php print $below_right_wrapper; ?> class="ds-below-right<?php print $below_right_classes; ?>">
      <?php print $below_right; ?>
    </<?php print $below_right_wrapper; ?>>
    </div>

    <<?php print $footer_wrapper; ?> class="ds-footer<?php print $footer_classes; ?>">
      <?php print $footer; ?>
    </<?php print $footer_wrapper; ?>>

</<?php print $layout_wrapper ?>>

<!-- Needed to activate display suite support on forms. -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
