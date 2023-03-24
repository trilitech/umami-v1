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

module CellStandard = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=110.->dp, ~marginLeft=10.->dp, ())
  }
})

module CellName = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=200.->dp, ())
  }
})

module CellSymbol = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=100.->dp, ())
  }
})

module CellAddress = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=380.->dp, ~flexDirection=#row, ~alignItems=#center, ~flexShrink=1., ())
  }
})

module CellTokenId = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=100.->dp, ~flexGrow=1., ~alignItems=#flexStart, ())
  }
})

module CellAction = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=68.->dp, ~alignItems=#flexEnd, ())
  }
})

let styles = {
  open Style
  StyleSheet.create({
    "kind": style(~paddingHorizontal=12.->dp, ()),
    "actions": style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ()),
    "iconOffset": style(~paddingLeft=14.->dp, ()),
    "copyButton": style(~marginRight=6.->dp, ()),
  })
}

module EditionModal = {
  @react.component
  let make = (~action, ~item, ~token) => {
    open TokensLibrary.Token
    let (openAction, closeAction, wrapModal) = ModalAction.useModal()

    <>
      {item(_ => openAction())}
      {wrapModal(
        <ContractAddView
          action
          address={token->address}
          kind={TokenContract.toTokenKind(token->kind, token->id)}
          chain={token->chain}
          closeAction
        />,
      )}
    </>
  }
}

module AddButton = {
  @react.component
  let make = (~token, ~chain) => {
    let disabled = chain == None

    let item = onPress => {
      let icon = (~color=?, ~style=?) =>
        Icons.SwitchOff.build(
          ~style=Style.arrayOption([style, styles["iconOffset"]->Some]),
          ~color?,
        )

      let tooltip = (
        "add_token_button" ++ token->TokensLibrary.Token.uniqueKey,
        I18n.Tooltip.add_to_wallet,
      )

      <IconButton disabled tooltip onPress icon size=46. iconSizeRatio=1.1 />
    }

    <EditionModal action=#Add item token />
  }
}

module MoreMenu = {
  module EditItem = {
    @react.component
    let make = (~token, ~chain) => {
      let disabled = chain == None

      let item = onPress =>
        <Menu.Item disabled text=I18n.Menu.see_metadata onPress icon=Icons.Search.build />

      <> <EditionModal action=#Edit item token /> </>
    }
  }

  module MultisigDetailsModal = {
    @react.component
    let make = (~multisig, ~chain) => {
      let (openAction, closeAction, wrapModal) = ModalAction.useModal()
      let disabled = chain == None
      <>
        <Menu.Item
          disabled text=I18n.Menu.see_details onPress={_ => openAction()} icon=Icons.Search.build
        />
        {wrapModal(<ContractDetailsView.Multisig multisig closeAction />)}
      </>
    }
  }

  module MultisigDeleteButton = {
    @react.component
    let make = (~address: PublicKeyHash.t) => {
      let (multisigRequest, deleteMultisig) = StoreContext.Multisig.useDelete()

      let onPressConfirmDelete = _e => deleteMultisig([address])

      <DeleteButton.MenuItem
        buttonText=I18n.Menu.delete_contract
        modalTitle=I18n.Title.delete_contract
        onPressConfirmDelete
        request=multisigRequest
      />
    }
  }

  module ContractLinkItem = {
    @react.component
    let make = (~address: PublicKeyHash.t, ~chain) => {
      let addToast = LogsContext.useToast()
      let disabled = chain == None

      let onPress = _ => {
        let explorer = // EndpointError will actually never been used, since the menu item is
        // disabled in that case
        chain->Option.mapDefault(Error(Network.EndpointError), chain =>
          chain->Network.fromChainId->Network.externalExplorer
        )
        switch explorer {
        | Ok(url) => System.openExternal(url ++ (address :> string))
        | Error(err) => addToast(Logs.error(~origin=Operation, err))
        }
      }

      <Menu.Item disabled icon=Icons.OpenExternal.build text=I18n.Menu.view_in_explorer onPress />
    }
  }

  module Token = {
    @react.component
    let make = (~token, ~chain) => {
      let keyPrefix = "tokenMenu-" ++ token->TokensLibrary.Token.uniqueKey
      <Menu icon=Icons.More.build size=46. iconSizeRatio=0.5 keyPopover=keyPrefix>
        [
          <EditItem key={keyPrefix ++ "-EditItem"} token chain />,
          <ContractLinkItem
            key={keyPrefix ++ "-ContractLinkItem"}
            address={token->TokensLibrary.Token.address}
            chain
          />,
        ]
      </Menu>
    }
  }

  module Multisig = {
    @react.component
    let make = (~multisig, ~chain) => {
      let keyPrefix = "multisigMenu-" ++ (multisig.Multisig.address :> string)
      <Menu icon=Icons.More.build size=46. iconSizeRatio=0.5 keyPopover=keyPrefix>
        [
          <MultisigDetailsModal key={keyPrefix ++ "-MultisigDetailsModal"} multisig chain />,
          <ContractLinkItem
            key={keyPrefix ++ "-ContractLinkItem"} address=multisig.Multisig.address chain
          />,
          <MultisigDeleteButton
            key={keyPrefix ++ "-MultisigDeleteButton"} address=multisig.Multisig.address
          />,
        ]
      </Menu>
    }
  }
}

module RemoveButton = {
  @react.component
  let make = (~token: TokensLibrary.Token.t, ~chain) => {
    let theme = ThemeContext.useTheme()
    let (_tokenRequest, deleteToken) = StoreContext.Tokens.useDelete(false)
    let disabled = chain == None

    let onPress = _e =>
      switch token {
      | Full(token) => deleteToken(token)->Promise.ignore
      | Partial(_, _, _) => ()
      }

    let icon = (~color as _=?, ~style=?) =>
      Icons.SwitchOn.build(
        ~style=Style.arrayOption([style, styles["iconOffset"]->Some]),
        ~color=theme.colors.iconPrimary,
      )

    let tooltip = ("remove_token" ++ token->TokensLibrary.Token.uniqueKey, I18n.Btn.delete_token)

    <IconButton disabled tooltip icon onPress iconSizeRatio=1.1 size=46. />
  }
}

module TokenActions = {
  @react.component
  let make = (~token, ~registered: bool, ~chain:option<Umami.Network.chainId>) =>
    registered
      ? <View style={styles["actions"]}>
          <MoreMenu.Token token chain /> <RemoveButton token chain />
        </View>
      : <AddButton token chain />
}

module MultisigActions = {
  @react.component
  let make = (~multisig, ~chain) =>
    <View style={styles["actions"]}> <MoreMenu.Multisig multisig chain /> </View>
}

module Base = {
  @react.component
  let make = (~style=?, ~kind, ~name=?, ~symbol=?, ~address, ~tokenId=?, ~uniqueKey, ~action) => {
    let theme = ThemeContext.useTheme()
    let addToast = LogsContext.useToast()
    <View
      style={Style.arrayOption([
        style,
        Style.style(~borderColor=theme.colors.stateDisabled, ~borderBottomWidth=1., ())->Some,
      ])}>
      <Table.Row.Base minHeight=44.>
        <CellStandard>
          <Tag contentStyle={styles["kind"]} fontSize=10.8 height=20. content={kind} />
        </CellStandard>
        <CellName>
          <Typography.Body1 numberOfLines=1>
            {name->Option.default("")->React.string}
          </Typography.Body1>
        </CellName>
        <CellSymbol>
          <Typography.Body1 numberOfLines=1>
            {symbol->Option.default("")->React.string}
          </Typography.Body1>
        </CellSymbol>
        <CellAddress>
          <Typography.Address numberOfLines=1> {address->React.string} </Typography.Address>
          <ClipboardButton
            copied=I18n.Log.address
            addToast
            tooltipKey={uniqueKey}
            data={address}
            style={styles["copyButton"]}
          />
        </CellAddress>
        <CellTokenId>
          <Typography.Body1 numberOfLines=1>
            {tokenId->Option.default("")->React.string}
          </Typography.Body1>
        </CellTokenId>
        <CellAction> {action} </CellAction>
      </Table.Row.Base>
    </View>
  }
}

module Token = {
  @react.component
  let make = (
    ~style=?,
    ~token: TokensLibrary.Token.t,
    ~registered: bool,
    ~chain: option<Umami.Network.chainId>,
  ) => {
    let tokenId = switch token->TokensLibrary.Token.kind {
    | #KFA1_2 => I18n.na
    | #KFA2 => token->TokensLibrary.Token.id->Int.toString
    }
    let (chain : option<Umami.Network.chainId>) = chain
    let action = <TokenActions registered token chain />
    <Base
      ?style
      kind={token->TokensLibrary.Token.kind->TokenContract.kindToString}
      name=?{token->TokensLibrary.Token.name}
      symbol=?{token->TokensLibrary.Token.symbol}
      address={(token->TokensLibrary.Token.address :> string)}
      tokenId
      uniqueKey={token->TokensLibrary.Token.uniqueKey}
      action
    />
  }
}

module Multisig = {
  @react.component
  let make = (~style=?, ~multisig: Multisig.t, ~chain) => {
    let action = <MultisigActions multisig chain />
    <Base
      ?style
      kind={"multisig"}
      name={multisig.alias}
      address={(multisig.address :> string)}
      uniqueKey={(multisig.address :> string)}
      action
    />
  }
}
