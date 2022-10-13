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
    "content": style(~marginTop=0.->dp, ()),
    "empty": style(~textAlign=#center, ()),
    "view": style(~minHeight=400.->dp, ()),
    "modal": style(
      ~width=642.->dp,
      ~paddingTop=45.->dp,
      ~paddingBottom=32.->dp,
      ~paddingHorizontal=55.->dp,
      (),
    ),
    "clear": style(~width=auto, ~alignSelf=#flexEnd, ~padding=5.->dp, ()),
  })
}

module ClearButton = {
  let styles = {
    open Style
    StyleSheet.create({
      "button": style(~alignSelf=#flexStart, ~marginLeft=-6.->dp, ~marginBottom=10.->dp, ()),
    })
  }

  @react.component
  let make = () => {
    let clearLogs = LogsContext.useClear()
    <View style={styles["button"]}>
      <ButtonAction text=I18n.logs_clearall onPress={_ => clearLogs()} icon=Icons.Close.build />
    </View>
  }
}

@react.component
let make = () => {
  let logs = LogsContext.useLogs()
  let deleteError = LogsContext.useDelete()
  let addLog = LogsContext.useAdd()
  let errors = logs->List.keep(({Logs.kind: kind}) => kind == Logs.Error)
  <Page>
    <View style={styles["content"]}>
      {ReactUtils.onlyWhen(<ClearButton />, errors != list{})}
      {switch errors {
      | list{} =>
        <Typography.Body1 style={styles["empty"]}>
          {I18n.logs_no_recent->React.string}
        </Typography.Body1>
      | errors =>
        errors
        ->List.toArray
        ->Array.mapWithIndex((i, log) =>
          <LogItem.Entry
            key={log.timestamp->Float.toString}
            indice=i
            log
            addToast={addLog(true)}
            handleDelete=deleteError
            isFirst={i == 0}
          />
        )
        ->React.array
      }}
    </View>
  </Page>
}
