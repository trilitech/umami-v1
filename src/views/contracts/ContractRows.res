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

module TableHeader = {
  module TableHeaderContent = {
    @react.component
    let make = (~text=?, ()) => {
      <Typography.Overline3>
        {Option.mapWithDefault(text, React.null, React.string)}
      </Typography.Overline3>
    }
  }

  module Standard = {
    @react.component
    let make = (~text=?, ()) =>
      <ContractRowItem.CellStandard> <TableHeaderContent ?text /> </ContractRowItem.CellStandard>
  }

  module Name = {
    @react.component
    let make = (~text=?, ()) =>
      <ContractRowItem.CellName> <TableHeaderContent ?text /> </ContractRowItem.CellName>
  }

  module Symbol = {
    @react.component
    let make = (~text=?, ()) =>
      <ContractRowItem.CellSymbol> <TableHeaderContent ?text /> </ContractRowItem.CellSymbol>
  }

  module Address = {
    @react.component
    let make = (~text=?, ()) =>
      <ContractRowItem.CellAddress> <TableHeaderContent ?text /> </ContractRowItem.CellAddress>
  }

  module TokenId = {
    @react.component
    let make = (~text=?, ()) =>
      <ContractRowItem.CellTokenId> <TableHeaderContent ?text /> </ContractRowItem.CellTokenId>
  }

  module Empty = {
    @react.component
    let make = () => <ContractRowItem.CellAction> React.null </ContractRowItem.CellAction>
  }

  module GenericHeader = {
    @react.component
    let make = (~standard="", ~name="", ~symbol="", ~address="", ~tokenid="", ()) => {
      <Table.Head>
        <Standard text=standard />
        <Name text=name />
        <Symbol text=symbol />
        <Address text=address />
        <TokenId text=tokenid />
        <Empty />
      </Table.Head>
    }
  }

  module Token = {
    @react.component
    let make = () =>
      <GenericHeader
        standard=I18n.token_column_standard
        name=I18n.token_column_name
        symbol=I18n.token_column_symbol
        address=I18n.token_column_address
        tokenid=I18n.token_column_tokenid
      />
  }

  module Multisig = {
    @react.component
    let make = () =>
      <GenericHeader
        standard=I18n.token_column_standard
        name=I18n.token_column_name
        address=I18n.token_column_address
      />
  }
}

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~marginBottom=16.->dp, ()),
    "header": style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ()),
    "headline": style(~fontSize=16., ()),
    "iconButton": style(~marginLeft=2.->dp, ~marginRight=2.->dp, ()),
  })
}

let makeTokenRowItem = (tokens, currentChain, (token, registered)) =>
  <ContractRowItem.Token
    key={token->TokensLibrary.Token.uniqueKey} token registered currentChain tokens
  />

module Collapsable = {
  //FIXME: Move it somewhere else
  @react.component
  let make = (~header, ~expanded=true, ~children) => {
    let (expanded, setExpanded) = React.useState(_ => expanded)

    let collapseButton =
      <IconButton
        icon={expanded
          ? (~color=?, ~style as _=?) =>
              Icons.ChevronDown.build(~color?, ~style=Buttons.styles["chevronUp"])
          : Icons.ChevronDown.build}
        iconSizeRatio=0.7
        size=36.
        onPress={_ => setExpanded(expanded => !expanded)}
        style={styles["iconButton"]}
      />

    let header = header(collapseButton)

    <Accordion style={styles["container"]} header expanded> children </Accordion>
  }
}

module Token = {
  @react.component
  let make = (~title, ~tokens, ~currentChain, ~emptyText) => {
    let header = collapseButton => {
      <View style={styles["header"]}>
        collapseButton
        <Typography.Headline style={styles["headline"]}>
          {title->React.string}
        </Typography.Headline>
      </View>
    }
    <Collapsable header>
      {tokens->TokensLibrary.Contracts.isEmpty
        ? emptyText->Option.mapDefault(React.null, text =>
            <Table.Empty> {text->React.string} </Table.Empty>
          )
        : <>
            <TableHeader.Token />
            <View>
              {tokens
              ->TokensLibrary.Generic.valuesToArray
              ->Array.map(makeTokenRowItem(tokens, currentChain))
              ->React.array}
            </View>
          </>}
    </Collapsable>
  }
}

let makeMultisigRowItem = (currentChain, multisig) =>
  <ContractRowItem.Multisig key={(multisig.address :> string)} multisig currentChain />

module Multisig = {
  @react.component
  let make = (~multisigs, ~currentChain, ~emptyText) => {
    if Array.length(multisigs) == 0 {
      <View style={styles["container"]}>
        <Table.Empty> {emptyText->React.string} </Table.Empty>
      </View>
    } else {
      <View style={styles["container"]}>
        <TableHeader.Multisig />
        <View> {multisigs->Array.map(makeMultisigRowItem(currentChain))->React.array} </View>
      </View>
    }
  }
}
