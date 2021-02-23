open ReactNative;

module StateLenses = [%lenses type state = {words: array(string)}];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

let stateField = StateLenses.Words;
let formField = VerifyMnemonicForm.ReSchema.Field(stateField);

let numInput = 6;

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
    })
  );

let createUniquesRandomInts = (~count, ~min, ~max) => {
  let randomNumbers = ref(Set.Int.empty);
  while (randomNumbers.contents->Set.Int.size < count) {
    randomNumbers :=
      randomNumbers.contents->Set.Int.add(Js.Math.random_int(min, max));
  };
  randomNumbers.contents->Set.Int.toArray;
};

let isEqualMnemonicWord = (value, mnemonic, verifyMnemonicIndexes, index) => {
  let word =
    mnemonic->Array.getExn(verifyMnemonicIndexes->Array.getExn(index));
  value == word;
};

[@react.component]
let make = (~mnemonic, ~onPressCancel, ~goNextStep) => {
  let verifyMnemonicIndexes =
    React.useRef(
      createUniquesRandomInts(
        ~count=numInput,
        ~min=0,
        ~max=mnemonic->Array.size - 1,
      ),
    ).
      current;

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
                      word->isEqualMnemonicWord(
                        mnemonic,
                        verifyMnemonicIndexes,
                        index,
                      )
                        ? None
                        : Some({
                            ReSchema.error: "Invalid word",
                            index,
                            name: "word",
                          })
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
        ({state: _}) => {
          goNextStep();
          None;
        },
      ~initialState={words: Array.make(numInput, "")},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    <View style=styles##wordsList>
      {form.state.values.words
       ->Array.mapWithIndex((index, word) =>
           <React.Fragment key={index->string_of_int}>
             <View style=styles##wordItem>
               <InputMnemonicWord
                 index
                 word
                 displayIndex={verifyMnemonicIndexes->Array.getExn(index)}
                 arrayUpdateByIndex={form.arrayUpdateByIndex}
                 getNestedFieldError={form.getNestedFieldError}
                 formField
                 stateField
               />
             </View>
             {index mod 2 == 0 ? <View style=styles##wordSpacer /> : React.null}
           </React.Fragment>
         )
       ->React.array}
    </View>
    <View style=FormStyles.formActionSpaceBetween>
      <Buttons.Form text=I18n.btn#back onPress=onPressCancel />
      <Buttons.SubmitPrimary
        text=I18n.btn#continue
        onPress=onSubmit
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
