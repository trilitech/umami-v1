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

module AddTokenButton = {
  let styles = {
    open Style
    StyleSheet.create({
      "button": style(~alignSelf=#flexStart, ~marginLeft=-6.->dp, ~marginBottom=10.->dp, ()),
    })
  }

  @react.component
  let make = (~tokens, ~chain=?) => {
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
      {
        wrapModal(
          <TokenAddView
            action=#Add
            chain={chain->Option.getWithDefault(Network.unsafeChainId(""))}
            tokens
            closeAction
          />,
        )
      }
    </>
  }
}

// FIXME: Do not use a modal or reimplement CreateMultisigView
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
      {
        let account = StoreContext.SelectedAccount.useGetAtInit()
        Option.mapWithDefault(account, React.null, account =>
          wrapModal(
            <ModalFormView style={styles["modal"]}>
              <CreateMultisigView account closeAction />
            </ModalFormView>,
          )
        )
      }
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
      buttons=[(I18n.Btn.token, Token), (I18n.Btn.multisig, Multisig)]
    />
  }
}

module TokensView = {
  @react.component
  let make = (~registered, ~unregistered, ~currentChain) => <>
    <ContractRows.Token title=I18n.Title.added_to_wallet tokens=registered currentChain emptyText=None />
    <ContractRows.Token
      title=I18n.Title.held tokens=unregistered currentChain emptyText=Some(I18n.empty_held_token)
    />
  </>
}

module MultisigsView = {
  @react.component
  let make = (~multisigs: array<Multisig.t>, ~currentChain) =>
    <ContractRows.Multisig multisigs currentChain emptyText=I18n.Expl.no_multisig_contract />
}

@react.component
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll()
  let apiVersion: option<Network.apiVersion> = StoreContext.useApiVersion()
  let (syncState, setSyncState) = React.useState(_ => Sync.NotInitiated)
  let (searched, setSearch) = React.useState(_ => "")
  let stop = React.useRef(false)

  let (contractType, setContractType) = React.useState(() => Token)

  let accounts = accounts->PublicKeyHash.Map.keysToList
  let request = fromCache => {
    open TokensApiRequest
    {
      request: {
        open Fungible
        {accounts: accounts, numberByAccount: Tzkt.requestPageSize}
      },
      fromCache: fromCache,
    }
  }

  // will be used to indicate a percentage of tokens loaded
  let onTokens = (~total, ~lastToken) => {
    let percentage = Int.toFloat(lastToken + 1) /. Int.toFloat(total) *. 100.
    setSyncState(x =>
      switch x {
      | Canceled(_) => Canceled(percentage)
      | _ => Loading(percentage)
      }
    )
  }
  let onStop = () => stop.current

  let (tokensRequest, getTokens) = StoreContext.Tokens.useFetchTokens(
    onTokens,
    onStop,
    accounts,
    request(true),
  )

  let tokens = switch tokensRequest {
  | NotAsked
  | Loading(None) =>
    None

  | Loading(Some(#Cached(tokens) | #Fetched(tokens, _)))
  | Done(Ok(#Cached(tokens) | #Fetched(tokens, _)), _) =>
    Some(Ok(tokens))

  | Done(Error(error), _) => Some(Error(error))
  }

  let loadToCanceled = () =>
    setSyncState(x =>
      switch x {
      | Loading(percentage) => Canceled(percentage)
      | _ => NotInitiated
      }
    )
  let loadToDone = () =>
    setSyncState(x =>
      switch x {
      | Loading(_) => Done
      | state => state
      }
    )

  React.useEffect1(() =>
    switch tokensRequest {
    | Done(Ok(#Fetched(_, _)), _) =>
      loadToDone()
      stop.current = false
      None

    | Done(Ok(#Cached(_)), _) =>
      setSyncState(_ => NotInitiated)
      None

    | Done(Error(_), _) =>
      loadToCanceled()
      None

    | _ => None
    }
  , [tokensRequest])

  let matchToken = (token, searched) => {
    open TokensLibrary.Token
    let searched = searched->Js.String.toLocaleLowerCase
    let matchValue = value => value->Js.String.toLocaleLowerCase->Js.String2.includes(searched)
    token->name->Option.mapDefault(false, matchValue) ||
      (token->symbol->Option.mapDefault(false, matchValue) ||
      (token->address :> string)->matchValue)
  }

  let partitionedTokens = React.useMemo1(
    () =>
      tokens->Option.map(tokens =>
        tokens->Result.map(tokens =>
          tokens->TokensLibrary.Generic.keepPartition((_, _, (t, reg)) =>
            t->TokensLibrary.Token.isNFT || !(t->matchToken(searched)) ? None : Some(reg)
          )
        )
      ),
    [tokens],
  )

  let currentChain = apiVersion->Option.map(v => v.chain)

  let onRefresh = () => {
    setSyncState(_ => Loading(0.))
    getTokens(request(false))->ignore
  }

  let onStop = () => {
    setSyncState(x =>
      switch x {
      | Loading(percentage) => Canceled(percentage)
      | state => state
      }
    )
    stop.current = true
  }

  <Page>
    <Typography.Headline style=Styles.title>
      {I18n.Title.contracts->React.string}
    </Typography.Headline>
    <SearchAndSync
      value=searched
      onValueChange={value => setSearch(_ => value)}
      placeholder=I18n.Input_placeholder.search_for_token
      onRefresh
      onStop
      syncState
      syncIcon=Icons.SyncNFT.build
      style={styles["header"]}
    />
    <View style={styles["actionBar"]}> <ContractTypeSwitch contractType setContractType /> </View>
    <View style={styles["actionBar"]}>
      <AddTokenButton
        chain=?currentChain
        tokens={tokens->Option.mapDefault(TokensLibrary.Generic.empty, t =>
          t->Result.getWithDefault(TokensLibrary.Generic.empty)
        )}
      />
      {
        <CreateNewMultisigButton chain=?currentChain />->ReactUtils.onlyWhen(contractType == Multisig)
      }
    </View>
    {switch partitionedTokens {
    | None => <LoadingView />
    | Some(Error(error)) => <ErrorView error />
    | Some(Ok((registered, unregistered))) =>
      contractType == Token
      ? <TokensView registered unregistered currentChain />
       : <MultisigsView multisigs=Multisig.test_data currentChain />
    }}
  </Page>
}
