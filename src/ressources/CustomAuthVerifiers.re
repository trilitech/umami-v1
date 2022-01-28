open ReCustomAuth;

/* Verifiers are maps stored on Torus blockchain which describe the relation
   between providers handlers (eg. google email) and a secret key.
   We share (aggregate) verifiers with Kukai Wallet. They registered their verifiers
   with our Providers's API credetials.
   */

let google = accountHandle =>
  triggerAggregateLoginParams(
    ~verifierIdentifier="kukai-google",
    ~aggregateVerifierType=`single_id_verifier,
    ~subVerifierDetailsArray=[|
      subVerifier(
        ~clientId=
          "1070572364808-d31nlkneam5ee6dr0tu28fjjbsdkfta5.apps.googleusercontent.com",
        // our public key to Google Auth API
        ~typeOfLogin=`google,
        ~verifier="desktop-umami",
        ~jwtParams=
          jwtParams(
            ~display=`popup,
            ~prompt=`consent,
            ~login_hint=?accountHandle,
            (),
          ),
        (),
      ),
    |],
  );

let getProviderName =
  fun
  | `google => "Google";
