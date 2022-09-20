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
    "container": style(~marginTop=30.->dp, ()),
    "totalAmount": style(~textAlign=#right, ()),
    "listLabelContainer": style(
      ~flexDirection=#row,
      ~alignItems=#center,
      ~justifyContent=#spaceBetween,
      (),
    ),
    "listLabel": style(~marginBottom=4.->dp, ()),
    "amount": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
    "summary": style(~marginTop=11.->dp, ()),
    "row": style(~flex=1., ~paddingVertical=12.->dp, ~flexDirection=#row, ()),
    "addTransaction": style(~marginBottom=10.->dp, ()),
    "notFirstRow": style(~borderTopWidth=1., ()),
    "num": style(~width=22.->dp, ~height=44.->dp, ~marginRight=6.->dp, ~textAlign=#right, ()),
    "parameters": style(~marginTop=8.->dp, ()),
    "parametersContainer": style(~flex=1., ()),
    "moreButton": style(~marginHorizontal=auto, ()),
    "account": style(~flex=1., ()),
    "csvFormat": style(~marginRight=10.->dp, ()),
  })
}

module CSVFilePicker = {
  @react.component
  let make = (~onAddCSVList) => {
    let addLog = LogsContext.useAdd()
    let tokens = StoreContext.Tokens.useGetAll()

    let onChange = fileTextContent => {
      let parsedCSV = fileTextContent->CSVEncoding.parseCSV(~tokens)
      switch parsedCSV {
      | Result.Ok(parsedCSV) => onAddCSVList(parsedCSV)
      | Result.Error(error) => addLog(true, Logs.error(~origin=Logs.Batch, error))
      }
    }

    <TextFilePicker text=I18n.Btn.load_file primary=true accept=".csv" onChange />
  }
}

module CSVFormatLink = {
  let onPress = _ =>
    System.openExternal(
      "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/wikis/doc/Batch%20File%20Format%20Specifications",
    )

  @react.component
  let make = () =>
    <ButtonAction
      onPress text=I18n.Btn.csv_format_link primary=true icon=Icons.OpenExternal.build
    />
}

@module("uuid") external genUuid: unit => string = "v4"
module BuildingBatchMenu = {
  @react.component
  let make = (~onEdit, ~onDelete, ~onDetails) =>
    <Menu
      style={styles["moreButton"]}
      icon=Icons.More.build
      keyPopover={// API should not require a key.
      "batchItem" ++
      genUuid()}>
      [
        onDetails->ReactUtils.mapOpt(onDetails =>
          <Menu.Item
            key=I18n.Menu.see_details
            text=I18n.Menu.see_details
            icon=Icons.MagnifierPlus.build
            onPress={_ => onDetails()}
          />
        ),
        <Menu.Item
          key=I18n.Menu.batch_delete
          text=I18n.Menu.batch_delete
          icon=Icons.Delete.build
          onPress={_ => onDelete()}
        />,
        onEdit->Option.mapWithDefault(React.null, onEdit =>
          <Menu.Item
            key=I18n.Menu.batch_edit
            text=I18n.Menu.batch_edit
            icon=Icons.Edit.build
            onPress={_ => onEdit()}
          />
        ),
      ]
    </Menu>
}
