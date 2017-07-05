<?php

class HttpClientBasicAuth implements HttpClientAuthentication {
  function __construct($username, $password = NULL) {
    $this->username = $username;
    $this->password = $password;
  }

  public function authenticate($request) {
    // Some API require only API-key with empty password.
    $request->options['curlopts'][CURLOPT_USERPWD] = (!empty($this->password)) ? "{$this->username}:{$this->password}" : $this->username;
  }
}
