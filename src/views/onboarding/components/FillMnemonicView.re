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
  let render = ft =>
    <Typography.Body1 style=FormStyles.selector##item>
      {ft->Bip39.Mnemonic.formatToString->React.string}
    </Typography.Body1>;
  let renderButton = (ft, _hasError) =>
    <View style=FormStyles.selector##button>
      {switch (ft) {
       | Some(ft) => render(ft)
       | None => render(Words24)
       }}
    </View>;

  let onValueChange = (setMnemonics, mnemonics, format) => {
    let vInt = format->Bip39.Mnemonic.formatToInt;

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
      <FormLabel
        style=FormStyles.selector##label
        label=I18n.Label.recovery_phrase_format
      />
      <Selector
        style=FormStyles.selector##selector
        items=Bip39.Mnemonic.([|Words24, Words21, Words18, Words15, Words12|])
        getItemKey={ft => ft->Bip39.Mnemonic.formatToInt->Int.toString}
        renderItem=render
        selectedValueKey={value->Bip39.Mnemonic.formatToInt->Int.toString}
        onValueChange
        renderButton
        keyPopover="MnemonicsFormatSelector"
      />
    </>;
  };
};

module StateLenses = [%lenses
  type state = {
    format: Bip39.Mnemonic.format,
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
                      Js.String.length(word) == 0 || !word->Bip39.included
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
        text=I18n.Btn.continue
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
