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

open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "wordsList": style(~flexDirection=`row, ~flexWrap=`wrap, ()),
      "wordItem":
        style(
          ~marginVertical=2.->dp,
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=40.->pct,
          (),
        ),
      "wordSpacer": style(~width=20.->dp, ()),
      "secondaryBtn": style(~marginTop=8.->dp, ()),
    })
  );

module FormatSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "item":
          style(
            ~marginLeft=16.->dp,
            ~flexDirection=`row,
            ~flexWrap=`wrap,
            (),
          ),
        "label": style(~marginBottom=4.->dp, ()),
        "selector": style(~marginBottom=24.->dp, ()),

        "button": style(~flex=1., ~paddingVertical=11.->dp, ()),
      })
    );

  type format =
    | Words24
    | Words15
    | Words12;
  let toInt =
    fun
    | Words24 => 24
    | Words15 => 15
    | Words12 => 12;

  let toString = ft => I18n.t#words(ft->toInt);

  let render = ft =>
    <Typography.Body1 style=styles##item>
      {ft->toString->React.string}
    </Typography.Body1>;
  let renderButton = (ft, _hasError) =>
    <View style=styles##button>
      {switch (ft) {
       | Some(ft) => render(ft)
       | None => render(Words24)
       }}
    </View>;

  let onValueChange = (setMnemonics, mnemonics, format) => {
    let vInt = format->toInt;

    let len = mnemonics->Array.length;

    let mnemonics =
      if (len > vInt) {
        mnemonics->Array.slice(~offset=0, ~len=vInt);
      } else if (len < vInt) {
        mnemonics->Array.concat(Array.make(vInt - len, ""));
      } else {
        mnemonics;
      };

    setMnemonics(format, mnemonics);
  };

  [@react.component]
  let make = (~value, ~onValueChange) => {
    <>
      <FormLabel style=styles##label label=I18n.label#recovery_phrase_format />
      <Selector
        style=styles##selector
        items=[|Words24, Words15, Words12|]
        getItemKey={ft => ft->toInt->Int.toString}
        renderItem=render
        selectedValueKey={value->toInt->Int.toString}
        onValueChange
        renderButton
        keyPopover="MnemonicsFormatSelector"
      />
    </>;
  };
};

module StateLenses = [%lenses
  type state = {
    format: FormatSelector.format,
    words: array(string),
  }
];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

let stateField = StateLenses.Words;
let formField = VerifyMnemonicForm.ReSchema.Field(stateField);

[@react.component]
let make =
    (
      ~mnemonic,
      ~setMnemonic,
      ~formatState,
      ~next,
      ~secondaryButton=?,
      ~nextSecondary=_ => (),
    ) => {
  let submitAction = React.useRef(`PrimarySubmit);

  let form: VerifyMnemonicForm.api =
    VerifyMnemonicForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        VerifyMnemonicForm.Validation.(
          Schema(
            custom(
              ({words}) => {
                let errors =
                  words
                  ->Array.mapWithIndex((index, word) => {
                      Js.String.length(word) == 0
                        ? Some({
                            ReSchema.error: "Invalid word",
                            index,
                            name: "word",
                          })
                        : None
                    })
                  ->Array.keepMap(e => e);

                let fieldState: ReSchema.fieldState =
                  errors->Array.size == 0 ? Valid : NestedErrors(errors);

                fieldState;
              },
              Words,
            ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          setMnemonic(_ => state.values.words);
          formatState->snd(_ => state.values.format);

          switch (submitAction.current) {
          | `PrimarySubmit => next()
          | `SecondarySubmit => nextSecondary()
          };

          None;
        },
      ~initialState={format: formatState->fst, words: mnemonic},
      (),
    );

  let onSubmit = (submitPath, _) => {
    submitAction.current = submitPath;
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    <FormatSelector
      value={form.values.format}
      onValueChange={format =>
        FormatSelector.onValueChange(
          (fmt, words) => {
            form.handleChange(Words, words);
            form.handleChange(Format, fmt);
          },
          form.values.words,
          format,
        )
      }
    />
    <DocumentContext.ScrollView showsVerticalScrollIndicator=true>
      <View style=styles##wordsList>
        {form.state.values.words
         ->Array.mapWithIndex((index, word) =>
             <React.Fragment key={index->string_of_int}>
               <View style=styles##wordItem>
                 <InputMnemonicWord
                   index
                   word
                   arrayUpdateByIndex={form.arrayUpdateByIndex}
                   getNestedFieldError={form.getNestedFieldError}
                   formField
                   stateField
                 />
               </View>
               {index mod 2 == 0
                  ? <View style=styles##wordSpacer /> : React.null}
             </React.Fragment>
           )
         ->(
             a =>
               a->Array.length mod 2 != 0
                 ? a->Array.concat([|<InputMnemonicWord.Dummy />|]) : a
           )
         ->React.array}
      </View>
    </DocumentContext.ScrollView>
    <View
      style=Style.(
        array([|
          FormStyles.verticalFormAction,
          style(~marginTop=32.->dp, ()),
        |])
      )>
      <Buttons.SubmitPrimary
        text=I18n.btn#continue
        onPress={onSubmit(`PrimarySubmit)}
        disabledLook={!formFieldsAreValids}
      />
      <View style=styles##secondaryBtn>
        {secondaryButton
         ->Option.map(f =>
             f(!formFieldsAreValids, onSubmit(`SecondarySubmit))
           )
         ->ReactUtils.opt}
      </View>
    </View>
  </>;
};
