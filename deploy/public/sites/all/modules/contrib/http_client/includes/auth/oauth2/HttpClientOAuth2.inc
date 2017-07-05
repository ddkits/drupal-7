<?php

class HttpClientOAuth2 implements HttpClientAuthentication {
  /**
   * The settings for the oauth2 client.
   *
   * An associative array that can have these items:
   *   - token_endpoint: something like https://server.org/oauth2/token
   *   - auth_flow: can be server-side|client-credentials|user-password)
   *   - client_id: the client ID as registered on the oauth2 server
   *   - client_secret: the client secret as registered on the oauth2 server
   *   - redirect_uri: something like https://client.org/oauth2/authorized
   *   - authorization_endpoint: like https://server.org/oauth2/authorize
   *   - username: username of resource owner on the oauth2 server
   *   - password: password of resource owner on the oauth2 server
   *   - scope: space separated list of scopes
   *   - query_auth: if TRUE, access_token will be set on the query,
   *                 otherwise on the headers
   *
   * See README.org for some examples.
   */
  private $settings;

  public function __construct($settings) {
    $this->settings = $settings;
  }

  /**
   * Gets an access_token from the oauth2 client and sets it to the $request.
   *
   * Used by the HttpClient to authenticate requests.
   *
   * @param HttpClientRequest $request
   * @return void
   */
  public function authenticate($request) {
    // Get an access_token.
    try {
      module_load_include('inc', 'oauth2_client', 'oauth2_client');
      $oauth2 = new OAuth2\Client($this->settings);
      $access_token = $oauth2->getAccessToken();
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
    }

    // Set the access_token to the request.
    $this->setAccessToken($request, $access_token);
  }

  /**
   * Set the access_token to the request.
   */
  protected function setAccessToken(&$request, $access_token) {
    $access_token_on_query = (isset($this->settings['query_auth']) ? $this->settings['query_auth'] : FALSE);

    if ($access_token_on_query) {
      // Send the access token in the query string.
      $request->parameters['access_token'] = $access_token;
    }
    else {
      // Send the access token in the 'Authorization' header.
      $request->setHeader('Authorization', 'Bearer ' . $access_token);
    }
  }
}
