<?php
/**
 * @file
 *  @copyright The Royal National Theatre
 *  @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 * 
 * This is so files can be read from the Finalcut server.
 * Images seems to be pretty easy as they use lots of functions that use streams
 */

/**
 * Drupal public (public://) stream wrapper class.
 *
 * Provides support for storing publicly accessible files with the Drupal file
 * interface.
 */
class FinalCutAudioPublicStreamWrapper extends DrupalLocalStreamWrapper {
  /**
   * Gets the Directory Path
   *
   * @return string
   *   File path
   */
  public function getDirectoryPath() {
    return variable_get('finalcut_audio_directory', conf_path() . '/files');
  }
  /**
   * Get External Url path
   *
   * @return string
   *   External Url path
   */
  public function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());
    if (strpos(self::getDirectoryPath(), ':') !== FALSE) {
       return self::getDirectoryPath() . '/' . drupal_encode_path($path);
    }
    else {
       return $GLOBALS['base_url'] . '/' . self::getDirectoryPath() . '/' . drupal_encode_path($path);
    }
  }
}
/**
 * NT public Image distribution system (finalcut-image://) stream wrapper class.
 *
 * Provides support for storing publicly accessible files with the Drupal file
 * interface.
 */
class FinalCutImagePublicStreamWrapper extends DrupalLocalStreamWrapper {
  /**
   * Gets the Directory Path.
   * For this we default to the public files for image cache purposes.
   *
   * @return string
   *   File path
   */
  public function getDirectoryPath() {
//    print '<pre>' . __LINE__ . ':' . __FILE__ . '</pre>';
    return variable_get('file_public_path', conf_path() . '/files');
  }
  /**
   * Get External Url path
   *
   * @return string
   *   External Url path
   */
  public function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());
    if (strpos($this->uri, 'finalcut-image://styles/') !== FALSE) {
      return $GLOBALS['base_url'] . '/' . self::getDirectoryPath() . '/'
        . drupal_encode_path($path);
    }
    else {
      return variable_get(
          'finalcut_image_directory',
          'http://media.nt-online.org/finalcut/image_exports'
          ) . '/' . drupal_encode_path($path);
    }
  }
  /**
   * Returns the canonical absolute path of the URI, if possible.
   * This is where the magic happens, in that for call to non style the module
   * path is returned, and for style it returns the public files dir
   *
   * @param string $uri
   *   (optional) The stream wrapper URI to be converted to a canonical
   *   absolute path. This may point to a directory or another type of file.
   *
   * @return string|false
   *   If $uri is not set, returns the canonical absolute path of the URI
   *   previously set by the DrupalStreamWrapperInterface::setUri() function.
   *   If $uri is set and valid for this class, returns its canonical absolute
   *   path, as determined by the realpath() function. If $uri is set but not
   *   valid, returns FALSE.
   */
  protected function getLocalPath($uri = NULL) {
    if (!isset($uri)) {
      $uri = $this->uri;
    }
    if (strpos($uri, 'finalcut-image://styles/') !== FALSE) {
      $uri = str_replace('finalcut-image:', 'public:', $uri);
      $path = $this->getDirectoryPath() . '/' . $this->getTarget($uri);
      $realpath = realpath($path);
      if (!$realpath) {
        // This file does not yet exist.
        $realpath = realpath(dirname($path)) . '/' . drupal_basename($path);
      }
      $directory = realpath($this->getDirectoryPath());
      if (!$realpath || !$directory || strpos($realpath, $directory) !== 0) {
        return FALSE;
      }
    }
    else {
      $realpath = variable_get(
          'finalcut_image_directory',
          'http://media.nt-online.org/finalcut/image_exports'
        ) 
        . '/' . $this->getTarget($uri);
    }
//    print '<pre>' . print_r($realpath, true) . '</pre>';
    return $realpath;
  }
  /**
   * Support for stat().
   *
   * @param $uri
   *   A string containing the URI to get information about.
   * @param $flags
   *   A bit mask of STREAM_URL_STAT_LINK and STREAM_URL_STAT_QUIET.
   *
   * @return
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   *
   * @see http://php.net/manual/en/streamwrapper.url-stat.php
   */
  public function url_stat($uri, $flags) {
    $this->uri = $uri;
    $path = $this->getLocalPath();
    // Suppress warnings if requested or if the file or directory does not
    // exist. This is consistent with PHP's plain filesystem stream wrapper.
    if (strpos($path, 'http://') !== FALSE || strpos($path, 'https://') !== FALSE) {
      if (function_exists('curl_init')) {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $path);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_HEADER, true);
        curl_setopt($ch, CURLOPT_NOBODY, true);
        // curl_setopt($ch, CURLOPT_HTTPPROXYTUNNEL, 1); 
        // curl_setopt($ch, CURLOPT_PROXY, 'localhost:3128'); 
        curl_exec($ch);
        $info = curl_getinfo($ch);
        curl_close($ch);
        if ($info['http_code'] == 200) {
          return array(
            0 => 0,
            1 => 0,
            2 => 33188,
            3 => 1,
            4 => 0,
            5 => 0,
            6 => 0,
            7 => $info['download_content_length'],
            8 => 0,
            9 => 0,
            10 => 0,
            11 => -1,
            12 => -1,
            'dev' => 0,
            'ino' => 0,
            'mode' => 33188,
            'nlink' => 1,
            'uid' => 0,
            'gid' => 0,
            'rdev' => 0,
            'size' => $info['download_content_length'],
            'atime' => 0,
            'mtime' => 0,
            'ctime' => 0,
            'blksize' => -1,
            'blocks' => -1,
          );
        }
      }
      else {
        watchdog(
          'ERROR',
          'You do not have curl installed so finalcut streams will not work'
        );
      }
      return FALSE;
    }
    else {
      if ($flags & STREAM_URL_STAT_QUIET || !file_exists($path)) {
      return @stat($path);
      }
      else {
        return stat($path);
      }
    }
  }
  /**
   * Support for fopen(), file_get_contents(), file_put_contents() etc.
   * @todo extend this to support proxy servers - as we the NT have one :(
   *
   * @param $uri
   *   A string containing the URI to the file to open.
   * @param $mode
   *   The file mode ("r", "wb" etc.).
   * @param $options
   *   A bit mask of STREAM_USE_PATH and STREAM_REPORT_ERRORS.
   * @param $opened_path
   *   A string containing the path actually opened.
   *
   * @return
   *   Returns TRUE if file was opened successfully.
   *
   * @see http://php.net/manual/en/streamwrapper.stream-open.php
   */
  public function stream_open($uri, $mode, $options, &$opened_path) {
    $this->uri = $uri;
    $path = $this->getLocalPath();
    //$opts = array('http' => array('proxy' => 'tcp://127.0.0.1:3128', 'request_fulluri' => true));
    $opts = array();
    $context = stream_context_create($opts);
    $this->handle = ($options & STREAM_REPORT_ERRORS) ? fopen($path, $mode, FALSE, $context) : @fopen($path, $mode, FALSE, $context);

    if ((bool) $this->handle && $options & STREAM_USE_PATH) {
      $opened_path = $path;
    }

    return (bool) $this->handle;
  }
}

class FinalCutVideoPublicStreamWrapper extends DrupalLocalStreamWrapper {
  /**
   * Gets the Directory Path
   *
   * @return string
   *   File path
   */
  public function getDirectoryPath() {
    return variable_get('finalcut_video_directory', conf_path() . '/files');
  }
  /**
   * Get External Url path
   *
   * @return string
   *   External Url path
   */
  public function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());
    if (strpos(self::getDirectoryPath(), ':') !== FALSE) {
       return self::getDirectoryPath() . '/' . drupal_encode_path($path);
    }
    else {
       return $GLOBALS['base_url'] . '/' . self::getDirectoryPath() . '/' . drupal_encode_path($path);
    }
  }
}
