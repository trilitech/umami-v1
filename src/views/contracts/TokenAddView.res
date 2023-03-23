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

open ReactNative

module StateLenses = %lenses(
  type state = {
    name: string,
    address: string,
    symbol: string,
    decimals: string,
    tokenId: string,
    token: option<TokensLibrary.Token.t>,
  }
)
module TokenCreateForm = ReForm.Make(StateLenses)

let styles = {
  open Style
  StyleSheet.create({
    "title": style(~marginBottom=6.->dp, ~textAlign=#center, ()),
    "overline": style(~marginBottom=24.->dp, ~textAlign=#center, ()),
    "tag": style(~marginBottom=6.->dp, ~alignSelf=#center, ()),
    "tagContent": style(~paddingHorizontal=12.->dp, ~fontSize=14., ~lineHeight=18., ()),
  })
}

module FormAddress = {
  @react.component
  let make = (~form: TokenCreateForm.api) =>
    <ContractAddView.FormAddress
      value=form.values.address
      handleChange={form.handleChange(Address)}
      error={list{
        form.formState->FormUtils.getFormStateError,
        form.getFieldError(Field(Address)),
      }->Option.firstSome}
    />
}

module FormTokenId = {
  @react.component
  let make = (~form: TokenCreateForm.api) => {
    let tooltipIcon =
      <IconButton
        icon=Icons.Info.build
        size=19.
        iconSizeRatio=1.
        tooltip=("formTokenId", I18n.Tooltip.tokenid)
        disabled=true
        style={
          open Style
          style(~borderRadius=0., ~marginLeft=4.->dp, ~marginTop=5.->dp, ())
        }
      />

    <FormGroupTextInput
      label=I18n.Label.add_token_id
      value=form.values.tokenId
      handleChange={form.handleChange(TokenId)}
      error={form.getFieldError(Field(TokenId))}
      placeholder=I18n.Input_placeholder.add_token_id
      tooltipIcon
    />
  }
}

module FormMetadata = {
  @react.component
  let make = (~form: TokenCreateForm.api) => <>
    <FormGroupTextInput
      label=I18n.Label.add_token_name
      value=form.values.name
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
      placeholder=I18n.Input_placeholder.add_token_name
    />
    <FormGroupTextInput
      label=I18n.Label.add_token_symbol
      value=form.values.symbol
      handleChange={form.handleChange(Symbol)}
      error={form.getFieldError(Field(Symbol))}
      placeholder=I18n.Input_placeholder.add_token_symbol
    />
    <FormGroupTextInput
      label=I18n.Label.add_token_decimals
      value=form.values.decimals
      handleChange={form.handleChange(Decimals)}
      error={form.getFieldError(Field(Decimals))}
      placeholder=I18n.Input_placeholder.add_token_decimals
    />
  </>
}

module MetadataForm = {
  @react.component
  let make = (~form: TokenCreateForm.api, ~pkh, ~kind, ~tokens) => {
    open TokensLibrary

    let token = MetadataApiRequest.useLoadMetadata(tokens, pkh, kind)

    React.useEffect1(() => {
      switch token {
      | Loading(Some(token))
      | Done(Ok(token), _) =>
        form.handleChange(Token, Some(token))
        form.handleChange(Name, token->Token.name->Option.default(""))
        form.handleChange(Symbol, token->Token.symbol->Option.default(""))
        form.handleChange(Decimals, token->Token.decimals->Option.mapDefault("", Int.toString))
      | Done(Error(_), _)
      | _ =>
        form.handleChange(Token, None)
      }
      None
    }, [token])
    switch token {
    | Done(Ok(_), _) => <FormMetadata form />
    | Done(Error(MetadataAPI.NoTzip12Metadata(_) | MetadataAPI.TokenIdNotFound(_, _)), _) =>
      <FormMetadata form />
    | Done(Error(_), _)
    | NotAsked => React.null
    | Loading(_) => <LoadingView />
    }
  }
}

type step =
  | Address
  | CheckContract
  | TokenId(PublicKeyHash.t)
  | Metadata(PublicKeyHash.t, TokenRepr.kind)
  | MultisigName(PublicKeyHash.t)

@react.component
let make = (
  ~action,
  ~chain,
  ~address: option<PublicKeyHash.t>=?,
  ~kind=?,
  ~tokens,
  ~cacheOnlyNFT=false,
  ~closeAction,
) => {
  let (tokenCreateRequest, createToken) = StoreContext.Tokens.useCreate()
  let (cacheTokenRequest, cacheToken) = StoreContext.Tokens.useCacheToken()
  let (contractKind, checkContract) = StoreContext.Contract.useCheck(tokens)
  let (step, setStep) = React.useState(_ => Address)

  let createToken = (token: TokenRepr.t) =>
    !(token->TokenRepr.isNFT)
      ? createToken(token)
      : cacheOnlyNFT
      ? cacheToken(token)
      : Promise.err(TokensAPI.RegisterNotAFungibleToken(token.address, token.kind))

  let (title, button) = switch action {
  | #Add => (I18n.Title.add_contract, I18n.Btn.validate)
  | #Edit => (I18n.Title.edit_metadata, I18n.Btn.update)
  }

  let onSubmit = ({state}: TokenCreateForm.onSubmitAPI) => {
    open TokenCreateForm
    switch step {
    | Metadata(address, kind) =>
      let alias = state.values.name
      let symbol = state.values.symbol
      let decimals = state.values.decimals |> Int.fromString |> Option.getExn
      state.values.token
      ->Option.flatMap(TokensLibrary.Token.toTokenRepr(~alias, ~symbol, ~decimals))
      ->Option.default({
        open TokenRepr
        {
          kind: kind,
          address: address,
          alias: alias,
          symbol: symbol,
          chain: chain,
          decimals: decimals,
          asset: Metadata.defaultAsset,
        }
      })
      ->createToken
      ->Promise.getOk(_ => closeAction())
      None
    | _ => None
    }
  }

  let form: TokenCreateForm.api = TokenCreateForm.use(
    ~schema={
      open TokenCreateForm.Validation
      Schema(nonEmpty(Name) + custom(state =>
          switch state.decimals->Int.fromString {
          | None => Error(I18n.Form_input_error.not_an_int)
          | Some(i) if i < 0 => Error(I18n.Form_input_error.negative_int)
          | Some(_) => Valid
          }
        , Decimals) + custom(state =>
          switch state.tokenId->Int.fromString {
          | None if state.tokenId == "" => Valid
          | None => Error(I18n.Form_input_error.not_an_int)
          | Some(i) if i < 0 => Error(I18n.Form_input_error.negative_int)
          // mouif
          | Some(_) => Valid
          }
        , Decimals) + custom(state =>
          switch PublicKeyHash.buildContract(state.address) {
          | Error(_) => Error(I18n.Form_input_error.invalid_key_hash)
          | Ok(_) => Valid
          }
        , Address) + custom(state =>
          switch state.token {
          | Some(t) if t->TokensLibrary.Token.isNFT && !cacheOnlyNFT =>
            Error(I18n.error_register_not_fungible)
          | _ => Valid
          }
        , TokenId) + nonEmpty(Symbol))
    },
    ~onSubmit,
    ~initialState={
      name: "",
      address: address->Option.mapDefault("", k => (k :> string)),
      symbol: "",
      decimals: "",
      tokenId: kind->Option.mapWithDefault("", k => k->TokenRepr.kindId->Int.toString),
      token: None,
    },
    ~i18n=FormUtils.i18n,
    (),
  )

  let onSubmit = _ => form.submit()

  let onValidPkh = pkh => {
    setStep(_ => CheckContract)
    pkh
    ->checkContract
    ->Promise.getOk(x =>
      switch x {
      | #KFA1_2 => setStep(_ => Metadata(pkh, TokenRepr.FA1_2))
      | #KFA2 => setStep(_ => TokenId(pkh))
      | #Multisig => setStep(_ => MultisigName(pkh))
      | #Unknown =>
        form.raiseSubmitFailed(I18n.Form_input_error.not_a_token_contract->Some)
        setStep(_ => Address)
      }
    )
  }

  let onValidTokenId = (pkh, tokenId) => setStep(_ => Metadata(pkh, TokenRepr.FA2(tokenId)))

  let pkh = PublicKeyHash.buildContract(form.values.address)
  let tokenId = form.values.tokenId->Int.fromString

  let headlineText = switch step {
  | Address
  | CheckContract => I18n.add_contract_sentence
  | TokenId(_) => I18n.add_token_contract_tokenid_fa2
  | Metadata(_, TokenRepr.FA2(_)) => I18n.add_token_contract_metadata_fa2
  | Metadata(_, TokenRepr.FA1_2) => I18n.add_token_contract_metadata_fa1_2
  | MultisigName(_) => I18n.add_token_contract_metadata_fa1_2
  }

  React.useEffect2(() => {
    switch (pkh, tokenId, step) {
    // Error case: the address becomes invalid
    | (Error(_), _, TokenId(_) | Metadata(_, _) | MultisigName(_)) => setStep(_ => Address)

    // Error case: the tokenId becomes invalid
    | (Ok(address), None, Metadata(_, TokenRepr.FA2(_))) => setStep(_ => TokenId(address))

    | (Ok(address), _, Address) => onValidPkh(address)

    | (_, Some(id), TokenId(pkh)) => onValidTokenId(pkh, id)

    | _ => ()
    }
    None
  }, (pkh, tokenId))

  let loading = tokenCreateRequest->ApiRequest.isLoading || cacheTokenRequest->ApiRequest.isLoading

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <ContractDetailsView.Title text=title />
    {
      open ApiRequest
      switch (contractKind, pkh) {
      | (Done(Ok(#...TokenContract.kind as kind), _), Ok(_)) =>
        <ContractDetailsView.Tag content={kind->TokenContract.kindToString} />
      | _ => React.null
      }
    }
    <ContractDetailsView.Overline text=headlineText />
    <FormAddress form />
    {switch step {
    | Address => React.null
    | CheckContract => <LoadingView />
    | TokenId(_) => <FormTokenId form />
    | Metadata(pkh, kind) => <>
        {kind != TokenRepr.FA1_2 ? <FormTokenId form /> : React.null}
        <MetadataForm form pkh kind tokens />
      </>
    | MultisigName(_) => <View />
    }}
    <Buttons.SubmitPrimary
      text=button
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>
}
