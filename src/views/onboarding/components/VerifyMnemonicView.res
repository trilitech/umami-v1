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

module StateLenses = %lenses(type state = {words: array<string>})

module VerifyMnemonicForm = ReForm.Make(StateLenses)

let stateField = StateLenses.Words
let formField = VerifyMnemonicForm.ReSchema.Field(stateField)

let numInput = 6

let styles = {
  open Style
  StyleSheet.create({
    "wordsList": style(~flexDirection=#row, ~flexWrap=#wrap, ()),
    "wordItem": style(
      ~marginVertical=2.->dp,
      ~flexGrow=1.,
      ~flexShrink=1.,
      ~flexBasis=40.->pct,
      (),
    ),
    "wordSpacer": style(~width=20.->dp, ()),
  })
}

let createUniquesRandomInts = (~count, ~min, ~max) => {
  let randomNumbers = ref(Set.Int.empty)
  while randomNumbers.contents->Set.Int.size < count {
    randomNumbers := randomNumbers.contents->Set.Int.add(Js.Math.random_int(min, max))
  }
  randomNumbers.contents->Set.Int.toArray
}

let isEqualMnemonicWord = (value, mnemonic, verifyMnemonicIndexes, index) => {
  let word = mnemonic->Array.getExn(verifyMnemonicIndexes->Array.getExn(index))
  value == word
}

@react.component
let make = (~mnemonic, ~goNextStep) => {
  let verifyMnemonicIndexes = React.useRef(
    createUniquesRandomInts(~count=numInput, ~min=0, ~max=mnemonic->Array.size - 1),
  ).current

  let form: VerifyMnemonicForm.api = VerifyMnemonicForm.use(
    ~validationStrategy=OnDemand,
    ~schema={
      open VerifyMnemonicForm.Validation
      Schema(custom(({words}) => {
          let errors =
            words
            ->Array.mapWithIndex((index, word) =>
              word->isEqualMnemonicWord(mnemonic, verifyMnemonicIndexes, index)
                ? None
                : Some({
                    ReSchema.error: "Invalid word",
                    index: index,
                    name: "word",
                  })
            )
            ->Array.keepMap(e => e)

          let fieldState: ReSchema.fieldState =
            errors->Array.size == 0 ? Valid : NestedErrors(errors)

          fieldState
        }, Words))
    },
    ~onSubmit=({state: _}) => {
      goNextStep()
      None
    },
    ~initialState={words: Array.make(numInput, "")},
    (),
  )

  let onSubmit = _ => form.submit()

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  <>
    <View style={styles["wordsList"]}>
      {form.state.values.words
      ->Array.mapWithIndex((index, word) =>
        <React.Fragment key={index->string_of_int}>
          <View style={styles["wordItem"]}>
            <InputMnemonicWord
              index
              word
              displayIndex={verifyMnemonicIndexes->Array.getExn(index)}
              arrayUpdateByIndex=form.arrayUpdateByIndex
              getNestedFieldError=form.getNestedFieldError
              formField
              stateField
            />
          </View>
          {mod(index, 2) == 0 ? <View style={styles["wordSpacer"]} /> : React.null}
        </React.Fragment>
      )
      ->React.array}
    </View>
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.Btn.continue onPress=onSubmit disabledLook={!formFieldsAreValids}
      />
    </View>
  </>
}
