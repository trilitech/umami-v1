/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "value": style(~marginLeft=5.->dp, ()),
    "uri": style(~textDecorationLine=#underline, ()),
    "field": style(~marginTop=24.->dp, ()),
    "subField": style(~flexDirection=#row, ~marginTop=5.->dp, ()),
    "modal": style(~width=832.->dp, ()),
  })
}

type fieldVal =
  | String(string)
  | Uri(string)

type fieldContent =
  | Mono(fieldVal)
  | Multi(array<(string, fieldVal)>)

let buildFields = (t: TokenRepr.t) => {
  Js.log(t.asset.formats)
  open List.Infix
  let formats =
    t.asset.formats->Option.flatMap(a =>
      a
      ->Array.get(0)
      ->Option.map(v =>
        \"@?"(
          v.mimeType->Option.map(mt => ("mimeType", String(mt))),
          \"@?"(v.uri->Option.map(uri => ("uri", Uri(uri))), list{}),
        )
      )
    )

  let creators =
    t.asset.creators->Option.map(creators =>
      creators->Array.mapWithIndex((i, cr) => (i->Int.toString, String(cr)))->Multi
    )

  let tokenId = switch t.kind {
  | FA1_2 => "none"
  | FA2(id) => id->Int.toString
  }

  let l = \"@:"(
    (I18n.Label.nft_spec_name, Mono(String(t.alias))),
    \"@:"(
      (I18n.Label.nft_spec_symbol, Mono(String(t.symbol))),
      \"@:"(
        (I18n.Label.nft_spec_contract_address, Mono(String((t.address :> string)))),
        \"@:"(
          (I18n.Label.nft_spec_token_id, Mono(String(tokenId))),
          \"@:"(
            (I18n.Label.nft_spec_decimals, Mono(String(t.decimals->Int.toString))),
            \"@:"(
              (
                I18n.Label.nft_spec_boolean_amount,
                Mono(String(t.asset.isBooleanAmount->Js.String.make)),
              ),
              \"@?"(
                creators->Option.map(c => (I18n.Label.nft_spec_creators, c)),
                \"@?"(
                  t.asset.thumbnailUri->Option.map(uri => (
                    I18n.Label.nft_spec_thumbnail_uri,
                    Mono(Uri(uri)),
                  )),
                  \"@?"(
                    t.asset.artifactUri->Option.map(uri => (
                      I18n.Label.nft_spec_artifact_uri,
                      Mono(Uri(uri)),
                    )),
                    \"@?"(
                      t.asset.displayUri->Option.map(uri => (
                        I18n.Label.nft_spec_display_uri,
                        Mono(Uri(uri)),
                      )),
                      \"@?"(
                        formats->Option.map(fmt => (
                          I18n.Label.nft_spec_formats,
                          Multi(fmt->List.toArray),
                        )),
                        \"@?"(
                          t.asset.description->Option.map(d => (
                            I18n.Label.nft_spec_description,
                            Mono(String(d)),
                          )),
                          list{},
                        ),
                      ),
                    ),
                  ),
                ),
              ),
            ),
          ),
        ),
      ),
    ),
  )

  l->List.toArray
}

let showUri = (~style as st=?, s) =>
  <Typography.URI
    onPress={_ =>
      Linking.openURL({
        open NftFilesManager
        s->toRessource->ressourceToURL
      })->ignore}
    style={
      open Style
      arrayOption([styles["uri"]->Some, st])
    }>
    {s->React.string}
  </Typography.URI>

let showVal = x =>
  switch x {
  | String(s) => <Text style={styles["value"]}> {s->React.string} </Text>
  | Uri(s) => showUri(~style=styles["value"], s)
  }

module Field = {
  @react.component
  let make = (~style=?, ~field as (field, v)) =>
    <View ?style>
      <Typography.Overline2> {field->React.string} </Typography.Overline2>
      {switch v {
      | Mono(Uri(s)) => showUri(s)
      | Mono(String(s)) =>
        <Typography.Body1 style={styles["subField"]}> {s->React.string} </Typography.Body1>
      | Multi(fields) =>
        fields
        ->Array.map(((field, v)) =>
          <Typography.Body1 key=field style={styles["subField"]}>
            {`${field}:`->React.string} {showVal(v)}
          </Typography.Body1>
        )
        ->React.array
      }}
    </View>
}

type state =
  | Metadata
  | Json

@react.component
let make = (~closeAction, ~nft: Token.t) => {
  let values = React.useMemo1(() => buildFields(nft), [nft])

  let (state, setState) = React.useState(() => Metadata)

  let json: option<string> = React.useMemo1(
    () => nft->TokenRepr.toFlatJson->Option.map(j => j->Js.Json.stringifyWithSpace(4)),
    [nft],
  )

  let headerActionButton = {
    let (icon, text) = switch state {
    | Metadata => (Icons.Code.build, I18n.Btn.json)
    | Json => (Icons.Metadata.build, I18n.Btn.metadata)
    }

    <ButtonAction
      icon
      text
      onPress={_ =>
        setState(x =>
          switch x {
          | Metadata => Json
          | Json => Metadata
          }
        )}
    />
  }

  <ModalFormView
    style={styles["modal"]}
    headerActionButton
    title=I18n.Title.nft_specs
    closing=ModalFormView.Close(closeAction)>
    {switch state {
    | Metadata =>
      <View>
        {values
        ->Array.mapWithIndex((i, field) => {
          let style = i == 0 ? None : Some(styles["field"])
          <Field key={i->Int.toString} ?style field />
        })
        ->React.array}
      </View>
    | Json => json->ReactUtils.mapOpt(text => <CodeView text />)
    }}
  </ModalFormView>
}
