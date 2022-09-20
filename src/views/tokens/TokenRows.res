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
  @react.component
  let make = () =>
    <Table.Head>
      <TokenRowItem.CellStandard>
        <Typography.Overline3> {I18n.token_column_standard->React.string} </Typography.Overline3>
      </TokenRowItem.CellStandard>
      <TokenRowItem.CellName>
        <Typography.Overline3> {I18n.token_column_name->React.string} </Typography.Overline3>
      </TokenRowItem.CellName>
      <TokenRowItem.CellSymbol>
        <Typography.Overline3> {I18n.token_column_symbol->React.string} </Typography.Overline3>
      </TokenRowItem.CellSymbol>
      <TokenRowItem.CellAddress>
        <Typography.Overline3> {I18n.token_column_address->React.string} </Typography.Overline3>
      </TokenRowItem.CellAddress>
      <TokenRowItem.CellTokenId>
        <Typography.Overline3> {I18n.token_column_tokenid->React.string} </Typography.Overline3>
      </TokenRowItem.CellTokenId>
      <TokenRowItem.CellAction> React.null </TokenRowItem.CellAction>
    </Table.Head>
}

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~marginBottom=16.->dp, ()),
    "header": style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ()),
    "headline": style(~fontSize=16., ()),
    "iconButton" /*  */: style(~marginLeft=2.->dp, ~marginRight=2.->dp, ()),
  })
}

let makeRowItem = (tokens, currentChain, (token, registered)) =>
  <TokenRowItem key={token->TokensLibrary.Token.uniqueKey} token registered currentChain tokens />

@react.component
let make = (~title, ~tokens, ~currentChain, ~emptyText) => {
  let (expanded, setExpanded) = React.useState(_ => true)

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

  let header =
    <View style={styles["header"]}>
      collapseButton
      <Typography.Headline style={styles["headline"]}> {title->React.string} </Typography.Headline>
    </View>

  tokens->TokensLibrary.Contracts.isEmpty
    ? emptyText->Option.mapDefault(React.null, text =>
        <Accordion style={styles["container"]} header expanded>
          <Table.Empty> {text->React.string} </Table.Empty>
        </Accordion>
      )
    : <Accordion style={styles["container"]} header expanded>
        <TableHeader />
        <View>
          {tokens
          ->TokensLibrary.Generic.valuesToArray
          ->Array.map(makeRowItem(tokens, currentChain))
          ->React.array}
        </View>
      </Accordion>
}
