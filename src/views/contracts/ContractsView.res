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

let styles = {
  open Style
  StyleSheet.create({
    "header": style(~marginBottom=8.->dp, ()),
    "actionBar": style(~flexDirection=#row, ()),
  })
}

module AddContractButton = {
  let styles = {
    open Style
    StyleSheet.create({
      "button": style(~alignSelf=#flexStart, ~marginLeft=-6.->dp, ~marginBottom=10.->dp, ()),
    })
  }

  @react.component
  let make = (~chain=?, ~popup) => {
    let (openAction, closeAction, wrapModal) = ModalAction.useModal()
    let tooltip =
      chain == None ? Some(("add_token_button", I18n.Tooltip.chain_not_connected)) : None

    <>
      <ButtonAction
        style={styles["button"]}
        disabled={chain == None}
        ?tooltip
        onPress={_ => openAction()}
        text=I18n.Btn.add_contract
        icon=Icons.Add.build
        primary=true
      />
      {wrapModal(popup(closeAction))}
    </>
  }
}

module AddTokenButton = {
  @react.component
  let make = (~tokens, ~chain=?) => {
    let popup = closeAction => {
      <TokenAddView
        action=#Add
        chain={chain->Option.getWithDefault(Network.unsafeChainId(""))}
        tokens
        closeAction
      />
    }
    <AddContractButton chain popup />
  }
}

module AddMultisigButton = {
  @react.component
  let make = (~chain=?) => {
    let popup = closeAction => {<MultisigAddView closeAction />}
    <AddContractButton chain popup />
  }
}

module CreateNewMultisigButton = {
  let styles = {
    open Style
    StyleSheet.create({
      "button": style(~alignSelf=#flexStart, ~marginLeft=10.->dp, ~marginBottom=10.->dp, ()),
      "modal": style(~paddingRight=0.->dp, ()),
    })
  }

  @react.component
  let make = (~chain=?) => {
    let (openAction, closeAction, wrapModal) = ModalAction.useModal()
    let tooltip =
      chain == None ? Some(("add_multisig_button", I18n.Tooltip.chain_not_connected)) : None

    <>
      <ButtonAction
        style={styles["button"]}
        disabled={chain == None}
        ?tooltip
        onPress={_ => openAction()}
        text=I18n.Btn.create_new_multisig
        icon=Icons.Key.build
        primary=true
      />
      {<CreateMultisigView closeAction />->wrapModal}
    </>
  }
}

type contractType = Token | Multisig

module ContractTypeSwitch = {
  @react.component
  let make = (~contractType, ~setContractType) => {
    <SegmentedButtons
      selectedValue=contractType
      setSelectedValue=setContractType
      buttons=[(I18n.Btn.token, Token, false), (I18n.Btn.multisig, Multisig, false)]
    />
  }
}

module TokensView = {
  @react.component
  let make = (~registered, ~unregistered, ~chain) => <>
    <ContractRows.Token title=I18n.Title.added_to_wallet tokens=registered chain emptyText=None />
    <ContractRows.Token
      title=I18n.Title.held tokens=unregistered chain emptyText=Some(I18n.empty_held_token)
    />
  </>
}

module MultisigsView = {
  @react.component
  let make = (~multisigs: array<Multisig.t>, ~chain) =>
    <ContractRows.Multisig multisigs chain emptyText=I18n.Expl.no_multisig_contract />
}

module Base = {
  @react.component
  let make = (
    ~contractState as (contractType, setContractType),
    ~searchState as (searched, setSearch),
    ~onRefresh,
    ~stop: React.ref<bool>,
    ~syncState as (syncState, setSyncState),
    ~children,
  ) => {
    let onRefresh = () => {
      setSyncState(_ => Sync.Loading(0.))
      onRefresh()
    }
    let onStop = () => {
      setSyncState(x =>
        switch x {
        | Sync.Loading(percentage) => Sync.Canceled(percentage)
        | state => state
        }
      )
      stop.current = true
    }
    <Page>
      {I18n.Title.contracts->Typography.headline(~style=Styles.title)}
      <SearchAndSync
        value=searched
        onValueChange={value => setSearch(_ => value)}
        placeholder=I18n.Input_placeholder.search_for_contract
        onRefresh
        onStop
        syncState
        syncIcon=Icons.SyncNFT.build
        style={styles["header"]}
      />
      <View style={styles["actionBar"]}> <ContractTypeSwitch contractType setContractType /> </View>
      {children}
    </Page>
  }
}

let loadToCanceled = setSyncState =>
  setSyncState(x =>
    switch x {
    | Sync.Loading(percentage) => Sync.Canceled(percentage)
    | _ => NotInitiated
    }
  )

let loadToDone = setSyncState =>
  setSyncState(x =>
    switch x {
    | Sync.Loading(_) => Sync.Done
    | state => state
    }
  )

module TokenPage = {
  @react.component
  let make = (
    ~contractState,
    ~searchState,
    ~syncState: (Sync.state, (Sync.state => Sync.state) => unit),
    ~stop: React.ref<bool>,
    ~chain=?,
  ) => {
    let accounts = StoreContext.Accounts.useGetAll()
    let accounts = accounts->PublicKeyHash.Map.keysToList
    let (_, setSyncState) = syncState
    let (searched, _) = searchState

    let tokensRequest = (fromCache): TokensApiRequest.withCache<
      TokensApiRequest.Fungible.request,
    > => {
      request: {accounts: accounts, numberByAccount: Tzkt.requestPageSize},
      fromCache: fromCache,
    }

    let (tokensApiRequest, tokensGetRequest) = {
      // will be used to indicate a percentage of tokens loaded
      let onTokens = (~total, ~lastToken) => {
        let percentage = Int.toFloat(lastToken + 1) /. Int.toFloat(total) *. 100.
        setSyncState(x =>
          switch x {
          | Sync.Canceled(_) => Sync.Canceled(percentage)
          | _ => Loading(percentage)
          }
        )
      }
      StoreContext.Tokens.useFetchTokens(
        onTokens,
        {() => stop.current},
        accounts,
        tokensRequest(true),
      )
    }

    let onRefresh = () => tokensGetRequest(tokensRequest(false))->ignore

    let tokens = switch tokensApiRequest {
    | NotAsked
    | Loading(None) =>
      None
    | Loading(Some(#Cached(tokens) | #Fetched(tokens, _)))
    | Done(Ok(#Cached(tokens) | #Fetched(tokens, _)), _) =>
      Some(Ok(tokens))
    | Done(Error(error), _) => Some(Error(error))
    }

    React.useEffect1(() =>
      switch tokensApiRequest {
      | Done(Ok(#Fetched(_, _)), _) =>
        loadToDone(setSyncState)
        stop.current = false
        None
      | Done(Ok(#Cached(_)), _) =>
        setSyncState(_ => NotInitiated)
        None
      | Done(Error(_), _) =>
        loadToCanceled(setSyncState)
        None
      | _ => None
      }
    , [tokensApiRequest])

    let partitionedTokens = {
      let matchToken = (token, searched) => {
        open TokensLibrary.Token
        let searched = searched->Js.String.toLocaleLowerCase
        let matchValue = value => value->Js.String.toLocaleLowerCase->Js.String2.includes(searched)
        token->name->Option.mapDefault(false, matchValue) ||
          (token->symbol->Option.mapDefault(false, matchValue) ||
          (token->address :> string)->matchValue)
      }
      tokens->Option.map(tokens =>
        tokens->Result.map(tokens =>
          tokens->TokensLibrary.Generic.keepPartition((_, _, (t, reg)) =>
            t->TokensLibrary.Token.isNFT || !(t->matchToken(searched)) ? None : Some(reg)
          )
        )
      )
    }

    <Base contractState searchState onRefresh stop syncState>
      <View style={styles["actionBar"]}>
        <AddTokenButton
          ?chain
          tokens={tokens->Option.mapDefault(TokensLibrary.Generic.empty, t =>
            t->Result.getWithDefault(TokensLibrary.Generic.empty)
          )}
        />
      </View>
      {switch partitionedTokens {
      | None => <LoadingView />
      | Some(Error(error)) => <ErrorView error />
      | Some(Ok((registered, unregistered))) => <TokensView registered unregistered chain />
      }}
    </Base>
  }
}

module MultisigPage = {
  @react.component
  let make = (~contractState, ~searchState, ~syncState, ~stop: React.ref<bool>, ~chain=?) => {
    let (_, setSyncState) = syncState
    let (searched, _) = searchState
    let multisigsApiRequest = StoreContext.Multisig.useRequest()

    let multisigs = switch multisigsApiRequest {
    | NotAsked
    | Loading(None) =>
      None
    | Done(Ok(multisigs), _)
    | Loading(Some(multisigs)) =>
      Some(Ok(multisigs))
    | Done(Error(error), _) => Some(Error(error))
    }

    React.useEffect1(() =>
      switch multisigsApiRequest {
      | Done(Ok(_), _) =>
        loadToDone(setSyncState)
        stop.current = false
        None
      | Done(Error(_), _) =>
        loadToCanceled(setSyncState)
        None
      | _ => None
      }
    , [multisigsApiRequest])

    let onRefresh = () => () // tokensGetRequest(tokensRequest(false))->ignore

    let filteredMultisigs = {
      let matchMultisig = (searched, multisig: Multisig.t) => {
        let searched = searched->Js.String.toLocaleLowerCase
        let matchValue = value => value->Js.String.toLocaleLowerCase->Js.String2.includes(searched)
        let matchPkh = (value: PublicKeyHash.t) => matchValue((value :> string))
        matchPkh(multisig.address) ||
        Array.some(multisig.Multisig.signers, matchPkh) ||
        matchValue(multisig.Multisig.alias)
      }
      multisigs->Option.map(multisigs =>
        multisigs->Result.map(multisigs =>
          PublicKeyHash.Map.keep(multisigs, (_, m) => matchMultisig(searched, m))
        )
      )
    }

    <Base contractState searchState onRefresh stop syncState>
      <View style={styles["actionBar"]}>
        <AddMultisigButton chain /> <CreateNewMultisigButton chain />
      </View>
      {switch filteredMultisigs {
      | None => <LoadingView />
      | Some(Error(error)) => <ErrorView error />
      | Some(Ok(multisigs)) =>
        <MultisigsView multisigs={Umami.PublicKeyHash.Map.valuesToArray(multisigs)} chain />
      }}
    </Base>
  }
}

@react.component
let make = () => {
  let apiVersion: option<Network.apiVersion> = StoreContext.useApiVersion()
  let syncState = React.useState(_ => Sync.NotInitiated)
  let searchState = React.useState(_ => "")
  let stop = React.useRef(false)
  let (contractType, _) as contractState = React.useState(() => Token)
  let chain = apiVersion->Option.map(v => v.chain)

  {
    switch contractType {
    | Token => <TokenPage contractState searchState syncState stop ?chain />
    | Multisig => <MultisigPage contractState searchState syncState stop ?chain />
    }
  }
}
