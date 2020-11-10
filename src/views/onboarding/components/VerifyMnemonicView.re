open ReactNative;

module StateLenses = [%lenses type state = {words: array(string)}];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

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
      "formAction":
        style(
          ~marginTop=28.->dp,
          ~flexDirection=`row,
          ~justifyContent=`center,
          (),
        ),
    })
  );

let createUniquesRandomInts = (~count, ~min, ~max) => {
  let randomNumbers = ref(Belt.Set.Int.empty);
  while (randomNumbers.contents->Belt.Set.Int.size < count) {
    randomNumbers :=
      randomNumbers.contents->Belt.Set.Int.add(Js.Math.random_int(min, max));
  };
  randomNumbers.contents->Belt.Set.Int.toArray;
};

let isEqualMnemonicWord = (value, mnemonic, verifyMnemonicIndexes, index) => {
  let word =
    mnemonic->Belt.Array.getExn(
      verifyMnemonicIndexes->Belt.Array.getExn(index),
    );
  let fieldState: ReSchema.fieldState =
    value == word ? Valid : Error("not the right word");
  fieldState;
};

[@react.component]
let make = (~mnemonic, ~onPressCancel, ~goNextStep) => {
  let verifyMnemonicIndexes =
    React.useRef(
      createUniquesRandomInts(
        ~count=numInput,
        ~min=0,
        ~max=mnemonic->Belt.Array.size - 1,
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
                  ->Belt.Array.mapWithIndex((index, word) => {
                      Js.String.length(word) == 0
                        ? Some({
                            ReSchema.error: "Invalid word",
                            index,
                            name: "word",
                          })
                        : None
                    })
                  ->Belt.Array.keepMap(e => e);

                let fieldState: ReSchema.fieldState =
                  errors->Belt.Array.size == 0 ? Valid : NestedErrors(errors);

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
      ~initialState={words: Belt.Array.make(numInput, "")},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <>
    <View style=Style.(array([|styles##wordsList, style(~zIndex=2, ())|]))>
      {form.state.values.words
       ->Belt.Array.mapWithIndex((index, word) =>
           <React.Fragment key={index->string_of_int}>
             <View
               style=Style.(
                 array([|
                   styles##wordItem,
                   style(
                     ~zIndex=form.state.values.words->Belt.Array.size - index,
                     (),
                   ),
                 |])
               )>
               <InputMnemonicWord
                 verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(index)}
                 value=word
                 handleChange={form.arrayUpdateByIndex(~field=Words, ~index)}
                 error={form.getNestedFieldError(Field(Words), index)}
               />
             </View>
             {index mod 2 == 0 ? <View style=styles##wordSpacer /> : React.null}
           </React.Fragment>
         )
       ->React.array}
    </View>
    <View style=Style.(array([|styles##formAction, style(~zIndex=1, ())|]))>
      <FormButton text="CANCEL" onPress=onPressCancel />
      <FormButton text="CONTINUE" onPress=onSubmit />
    </View>
  </>;
};
