/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

type spsk = private string

type t

type network = [#mainnet | #testnet]

@deriving(abstract)
type makeParams = {
  baseUrl: string,
  @optional
  redirectPathName: string,
  @optional
  redirectToOpener: bool,
  @optional
  enableLogging: bool,
  network: network,
}

@deriving(abstract)
type initParams = {
  @optional
  skipSw: bool,
}

type handledProvider = [#google]

type provider = [
  | handledProvider
  | #twitch
  | #discord
  | #github
  | #twitter
  | #reddit
  | #facebook
]

let providerToString: handledProvider => string = x =>
  switch x {
  | #google => "google"
  }

let getProviderName = x =>
  switch x {
  | #google => "Google"
  }

type Errors.t +=
  | UnableToRetrieveHandle
  | InvalidProvider(string)

let () = Errors.registerHandler("CustomAuth", x =>
  switch x {
  | InvalidProvider(s) => I18n.Errors.invalid_provider(s)->Some
  | UnableToRetrieveHandle => I18n.Errors.customauth_unable_to_retrieve_handle->Some
  | _ => None
  }
)

let providerFromString: string => Promise.result<handledProvider> = x =>
  switch x {
  | "google" => #google->Ok
  | s => Error(InvalidProvider(s))
  }

module Handle: {
  type t = private string
  let fromString: string => t
  let display: (t, handledProvider) => string

  let resolve: (~name: option<t>, ~email: option<t>) => Promise.result<t>
} = {
  type t = string
  let fromString = x => x

  let display = (name: t, provider) =>
    switch provider {
    | #google => name
    }

  let resolve = (~name, ~email) =>
    switch (email, name) {
    | (None | Some(""), None | Some("")) => Error(UnableToRetrieveHandle)
    | (None | Some(""), Some(name)) => Ok(name)
    | (Some(email), _) => Ok(email)
    }
}

module Verifier: {
  type t = private string
  let fromString: string => t
} = {
  type t = string
  let fromString = x => x
}

type prompt = [#none | #login | #consent | #select_account]
type display = [#page | #popup | #touch | #wap]

@deriving(abstract)
type jwtParams = {
  @optional
  domain: string,
  @optional
  login_hint: Handle.t,
  @optional
  prompt: prompt,
  @optional
  display: display,
}

type lookupInfos = {
  verifier: Verifier.t,
  verifierId: Handle.t,
}

@deriving(abstract)
type subVerifier = {
  clientId: string,
  typeOfLogin: provider,
  verifier: Verifier.t,
  @optional
  jwtParams: jwtParams,
}

type aggregateVerifier = [#single_id_verifier]

@deriving(abstract)
type triggerAggregateLoginParams = {
  aggregateVerifierType: aggregateVerifier,
  verifierIdentifier: Verifier.t,
  subVerifierDetailsArray: array<subVerifier>,
}

type userInfo = {
  email: option<Handle.t>,
  name: option<Handle.t>,
  typeOfLogin: handledProvider,
}

type aggregateLoginDetails = {
  privateKey: spsk,
  publicKey: string,
  userInfo: array<userInfo>,
}

let findInfo = (infos, provider: handledProvider) =>
  infos->Array.getBy(i => i.typeOfLogin == provider)->Result.fromOption(UnableToRetrieveHandle)

type infos = {
  provider: handledProvider,
  handle: Handle.t,
}

module Msg = {
  type instanceParams = {
    instanceId: string,
    verifier: string,
    typeOfLogin: string,
    redirectToOpener: bool,
  }

  type hashParams = {
    state: string,
    access_token: string,
    token_type: string,
    expires_in: string,
    scope: string,
    id_token: string,
    authuser: string,
    hd: string,
    prompt: string,
  }

  type data = {
    instanceParams: instanceParams,
    hashParams: hashParams,
  }

  type t = {
    channel: string,
    data: data,
    error: string,
  }
}
