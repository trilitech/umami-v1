open ReactNative;

module StateLenses = [%lenses
  type state = {
    word1: string,
    word2: string,
    word3: string,
    word4: string,
    word5: string,
    word6: string,
  }
];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

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
        ~count=6,
        ~min=0,
        ~max=mnemonic->Belt.Array.size - 1,
      ),
    ).
      current;

  let form: VerifyMnemonicForm.api =
    VerifyMnemonicForm.use(
      ~schema={
        VerifyMnemonicForm.Validation.(
          Schema(
            custom(
              values =>
                isEqualMnemonicWord(
                  values.word1,
                  mnemonic,
                  verifyMnemonicIndexes,
                  0,
                ),
              Word1,
            )
            + custom(
                values =>
                  isEqualMnemonicWord(
                    values.word2,
                    mnemonic,
                    verifyMnemonicIndexes,
                    1,
                  ),
                Word2,
              )
            + custom(
                values =>
                  isEqualMnemonicWord(
                    values.word3,
                    mnemonic,
                    verifyMnemonicIndexes,
                    2,
                  ),
                Word3,
              )
            + custom(
                values =>
                  isEqualMnemonicWord(
                    values.word4,
                    mnemonic,
                    verifyMnemonicIndexes,
                    3,
                  ),
                Word4,
              )
            + custom(
                values =>
                  isEqualMnemonicWord(
                    values.word5,
                    mnemonic,
                    verifyMnemonicIndexes,
                    4,
                  ),
                Word5,
              )
            + custom(
                values =>
                  isEqualMnemonicWord(
                    values.word6,
                    mnemonic,
                    verifyMnemonicIndexes,
                    5,
                  ),
                Word6,
              ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          Js.log(state);

          goNextStep();

          None;
        },
      ~initialState={
        word1: "",
        word2: "",
        word3: "",
        word4: "",
        word5: "",
        word6: "",
      },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <>
    <View style=Style.(array([|styles##wordsList, style(~zIndex=2, ())|]))>
      <View style=Style.(array([|styles##wordItem, style(~zIndex=6, ())|]))>
        <InputMnemonicWord
          verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(0)}
          value={form.values.word1}
          handleChange={form.handleChange(Word1)}
          error={form.getFieldError(Field(Word1))}
        />
      </View>
      <View style=styles##wordSpacer />
      <View style=Style.(array([|styles##wordItem, style(~zIndex=5, ())|]))>
        <InputMnemonicWord
          verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(1)}
          value={form.values.word2}
          handleChange={form.handleChange(Word2)}
          error={form.getFieldError(Field(Word2))}
        />
      </View>
      <View style=Style.(array([|styles##wordItem, style(~zIndex=4, ())|]))>
        <InputMnemonicWord
          verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(2)}
          value={form.values.word3}
          handleChange={form.handleChange(Word3)}
          error={form.getFieldError(Field(Word3))}
        />
      </View>
      <View style=styles##wordSpacer />
      <View style=Style.(array([|styles##wordItem, style(~zIndex=3, ())|]))>
        <InputMnemonicWord
          verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(3)}
          value={form.values.word4}
          handleChange={form.handleChange(Word4)}
          error={form.getFieldError(Field(Word4))}
        />
      </View>
      <View style=Style.(array([|styles##wordItem, style(~zIndex=2, ())|]))>
        <InputMnemonicWord
          verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(4)}
          value={form.values.word5}
          handleChange={form.handleChange(Word5)}
          error={form.getFieldError(Field(Word5))}
        />
      </View>
      <View style=styles##wordSpacer />
      <View style=Style.(array([|styles##wordItem, style(~zIndex=1, ())|]))>
        <InputMnemonicWord
          verifyIndex={verifyMnemonicIndexes->Belt.Array.getExn(5)}
          value={form.values.word6}
          handleChange={form.handleChange(Word6)}
          error={form.getFieldError(Field(Word6))}
        />
      </View>
    </View>
    <View style=Style.(array([|styles##formAction, style(~zIndex=1, ())|]))>
      <FormButton text="CANCEL" onPress=onPressCancel />
      <FormButton
        text="CONTINUE"
        onPress=onSubmit
        disabled={form.formState == Errored}
      />
    </View>
  </>;
};
