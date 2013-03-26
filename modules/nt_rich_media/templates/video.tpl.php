<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */
?>
<video id="player1" width="660" height="371" poster="<?php print $cover_img; ?>" controls="controls" preload="none">
  <?php if ($mp4): ?>
      <!-- MP4 for Safari, IE9, iPhone, iPad, Android, and Windows Phone 7 -->
      <source type="video/mp4" src="<?php print $mp4; ?>" />
  <?php endif; ?>
  <?php if ($webm): ?>
      <!-- WebM/VP8 for Firefox4, Opera, and Chrome -->
      <source type="video/webm" src="<?php print $webm; ?>" />
  <?php endif; ?>
  <?php if ($swf): ?>
      <!-- Flash fallback for non-HTML5 browsers without JavaScript -->
      <object width="960" height="540" type="application/x-shockwave-flash" data="<?php print $swf; ?>">
              <param name="movie" value="<?php print $swf; ?>" />
              <param name="flashvars" value="controls=true&amp;poster=<?php print $cover_img; ?>&amp;file=<?php print $mp4; ?>" />
              <!-- Image as a last resort -->
              <img src="<?php print $cover_img; ?>" width="960" height="540" title="No video playback capabilities" alt="No video playback capabilities" />
      </object>
  <?php endif; ?>
</video>
