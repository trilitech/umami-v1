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

[@bs.module "bip39"] external generate: int => string = "generateMnemonic";

[@bs.module "bip39"] [@bs.scope "wordlists"]
external wordlistsEnglish: array(string) = "english";

let set = HashSet.String.fromArray(wordlistsEnglish);

let included = w => {
  HashSet.String.has(set, w);
};

module Mnemonic = {
  type Errors.t +=
    | IncorrectNumberOfWords
    | UnknownWord(string, int);

  let () =
    Errors.registerHandler(
      "Bip39",
      fun
      | IncorrectNumberOfWords => I18n.errors#incorrect_number_of_words->Some
      | UnknownWord(w, i) => I18n.errors#unknown_bip39_word(w, i)->Some
      | _ => None,
    );

  type format =
    | Words24
    | Words21
    | Words18
    | Words15
    | Words12;
  let formatToInt =
    fun
    | Words24 => 24
    | Words21 => 21
    | Words18 => 18
    | Words15 => 15
    | Words12 => 12;

  let formatOfInt =
    fun
    | 24 => Words24->Ok
    | 21 => Words21->Ok
    | 18 => Words18->Ok
    | 15 => Words15->Ok
    | 12 => Words12->Ok
    | _ => Error(IncorrectNumberOfWords);

  let formatToString = ft => I18n.t#words(ft->formatToInt);

  let isStandardLength = i => i->formatOfInt->Result.isOk;
};
