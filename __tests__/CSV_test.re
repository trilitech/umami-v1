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
%raw
"
const util = require('util');
// enable full object reproting in console.log
util.inspect.defaultOptions.depth = null;
";

open TestFramework;
open CSVParser;

let checkResultList = (parsed, expected, equal) =>
  Result.map2(parsed, expected, (parsed, expected) =>
    List.every2(parsed, expected, equal)
  )
  ->Result.getWithDefault(false);

describe("CSV.Encodings.Wellformedness", ({test}) => {
  test("Cell wellformed", ({expectTrue}) => {
    let encoding = Encodings.(cell(string));
    expectTrue(Encodings.wellformed(encoding));
  });
  test("Tup2 wellformed", ({expectTrue}) => {
    let encoding = Encodings.(tup2(string, string));
    expectTrue(Encodings.wellformed(encoding));
  });
  test("Tup3 wellformed", ({expectTrue}) => {
    let encoding = Encodings.(tup3(string, string, string));
    expectTrue(Encodings.wellformed(encoding));
  });
  test("Tup4 wellformed", ({expectTrue}) => {
    let encoding = Encodings.(tup4(string, string, string, string));
    expectTrue(Encodings.wellformed(encoding));
  });
  test("Tup5 wellformed", ({expectTrue}) => {
    let encoding = Encodings.(tup5(string, string, string, string, string));
    expectTrue(Encodings.wellformed(encoding));
  });
  test("merge_rows wellformed", ({expectTrue}) => {
    let encoding =
      Encodings.(
        merge_rows(tup3(string, string, string), tup2(string, string))
      );
    expectTrue(Encodings.wellformed(encoding));
  });

  test("Or wellformed", ({expectTrue}) => {
    let encoding = Encodings.(or_(cell(number), cell(string)));
    expectTrue(Encodings.wellformed(encoding));
  });

  test("Or wellformed: only or, with null", ({expectTrue}) => {
    let encoding = Encodings.(or_(null, tup2(string, string)));
    expectTrue(Encodings.wellformed(encoding));
  });

  test("Or wellformed: in tup, same length", ({expectTrue}) => {
    let encoding =
      Encodings.(or_(tup2(string, string), tup2(string, string)));
    expectTrue(Encodings.wellformed(encoding));
  });

  test("Or illformed: in tup, different length", ({expectFalse}) => {
    let encoding =
      Encodings.(
        merge_rows(
          or_(tup3(string, string, string), tup2(string, string)),
          cell(string),
        )
      );
    expectFalse(Encodings.wellformed(encoding));
  });

  test("Or in or, same length", ({expectTrue}) => {
    let encoding =
      Encodings.(or_(cell(number), or_(cell(bool), cell(string))));
    expectTrue(Encodings.wellformed(encoding));
  });
  test("Or in or, different length", ({expectTrue}) => {
    let encoding =
      Encodings.(
        or_(cell(number), or_(cell(bool), tup2(string, string)))
      );
    expectTrue(Encodings.wellformed(encoding));
  });
});

describe("CSV.Cells", ({test}) => {
  test("Parse string", ({expectEqual}) => {
    let value = "umami";
    let encoding = Encodings.(cell(string)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    expectEqual(parsed, Ok([value]));
  });

  test("Parse bool", ({expectEqual}) => {
    let value = "true";
    let encoding = Encodings.(cell(bool)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    expectEqual(parsed, Ok([bool_of_string(value)]));
  });

  test("Parse number", ({expectTrue}) => {
    let value = "58610";
    let encoding = Encodings.(cell(number)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected = Ok([value->ReBigNumber.fromString]);
    expectTrue(checkResultList(parsed, expected, ReBigNumber.isEqualTo));
  });

  test("Parse optional number", ({expectEqual}) => {
    let value = "umami";
    let encoding = Encodings.(cell(opt(string))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    expectEqual(parsed, Ok([Some(value)]));
  });
});

describe("CSV.Custom", ({test}) => {
  test("Parse address", ({expectEqual}) => {
    let value = "tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH";
    let encoding =
      Encodings.(cell(custom(~conv=PublicKeyHash.buildImplicit))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    expectEqual(
      parsed,
      Ok([value->PublicKeyHash.buildImplicit->Result.getExn]),
    );
  });

  test("Parse contract", ({expectEqual}) => {
    let value = "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton";
    let encoding =
      Encodings.(cell(custom(~conv=PublicKeyHash.buildContract))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    expectEqual(
      parsed,
      Ok([value->PublicKeyHash.buildContract->Result.getExn]),
    );
  });

  test("Parse invalid address", ({expectTrue}) => {
    let value = "tz1InvalidAddress";
    let encoding =
      Encodings.(cell(custom(~conv=PublicKeyHash.buildImplicit))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let checkError =
      switch (parsed) {
      | Error(
          CSVParser.CannotParseCustomValue(
            ReTaquitoUtils.Invalid_checksum,
            _,
            _,
          ),
        ) =>
        true
      | _ => false
      };
    expectTrue(checkError);
  });

  test("Parse contract as implicit address (invalid)", ({expectTrue}) => {
    let value = "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton";
    let encoding =
      Encodings.(cell(custom(~conv=PublicKeyHash.buildImplicit))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let checkError =
      switch (parsed) {
      | Error(
          CSVParser.CannotParseCustomValue(
            PublicKeyHash.NotAnImplicit(_),
            _,
            _,
          ),
        ) =>
        true
      | _ => false
      };
    expectTrue(checkError);
  });

  test("Parse invalid contract", ({expectTrue}) => {
    let value = "KT1InvalidContract";
    let encoding =
      Encodings.(cell(custom(~conv=PublicKeyHash.buildContract))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let checkError =
      switch (parsed) {
      | Error(
          CSVParser.CannotParseCustomValue(
            ReTaquitoUtils.Invalid_checksum,
            _,
            _,
          ),
        ) =>
        true
      | _ => false
      };
    expectTrue(checkError);
  });

  test("Parse address as contract (invalid)", ({expectTrue}) => {
    let value = "tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH";
    let encoding =
      Encodings.(cell(custom(~conv=PublicKeyHash.buildContract))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let checkError =
      switch (parsed) {
      | Error(
          CSVParser.CannotParseCustomValue(
            PublicKeyHash.NotAContract(_),
            _,
            _,
          ),
        ) =>
        true
      | _ => false
      };
    expectTrue(checkError);
  });

  test("Parse Micheline", ({expectEqual}) => {
    let micheline =
      Encodings.custom(
        ~conv=ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline,
      );
    let encoding = Encodings.(cell(micheline)->mkRow);
    let rawValue = "(Pair 2 3)";
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(rawValue, encoding));
    let value =
      rawValue
      ->ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline
      ->Result.getExn;
    expectEqual(parsed, Ok([value]));
  });

  test("Parse invalid Micheline", ({expectTrue}) => {
    let micheline =
      Encodings.custom(
        ~conv=ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline,
      );
    let encoding = Encodings.(cell(micheline)->mkRow);
    let rawValue = "?";
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(rawValue, encoding));
    let checkError =
      switch (parsed) {
      | Error(
          CSVParser.CannotParseCustomValue(
            ReTaquitoError.ParseMicheline(_),
            _,
            _,
          ),
        ) =>
        true
      | _ => false
      };
    expectTrue(checkError);
  });
});

describe("CSV.Rows", ({test}) => {
  test("Parse tup2", ({expectTrue}) => {
    let value = "4235, test";
    let encoding = Encodings.(tup2(number, string)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected = Ok([("4235"->ReBigNumber.fromString, "test")]);
    let check = ((n1, s1), (n2, s2)) =>
      n1->ReBigNumber.isEqualTo(n2) && s1 == s2;
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse tup3", ({expectTrue}) => {
    let value = "4235, test, true";
    let encoding = Encodings.(tup3(number, string, bool)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected = Ok([("4235"->ReBigNumber.fromString, "test", true)]);
    let check = ((n1, s1, b1), (n2, s2, b2)) =>
      n1->ReBigNumber.isEqualTo(n2) && s1 == s2 && b1 == b2;
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse tup4", ({expectTrue}) => {
    let value = "4235, test, true, test2";
    let encoding = Encodings.(tup4(number, string, bool, string)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([("4235"->ReBigNumber.fromString, "test", true, "test2")]);
    let check = ((n1, s1, b1, s1'), (n2, s2, b2, s2')) =>
      n1->ReBigNumber.isEqualTo(n2) && s1 == s2 && b1 == b2 && s1' == s2';
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse tup5", ({expectTrue}) => {
    let value = "4235, test, true, test2, false";
    let encoding =
      Encodings.(tup5(number, string, bool, string, bool)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([("4235"->ReBigNumber.fromString, "test", true, "test2", false)]);
    let check = ((n1, s1, b1, s1', b1'), (n2, s2, b2, s2', b2')) =>
      n1->ReBigNumber.isEqualTo(n2)
      && s1 == s2
      && b1 == b2
      && s1' == s2'
      && b1' == b2';
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse multiline", ({expectTrue}) => {
    let value = "4235, test\n22, test2\n59, test3";
    let encoding = Encodings.(tup2(number, string)->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([
        ("4235"->ReBigNumber.fromString, "test"),
        ("22"->ReBigNumber.fromString, "test2"),
        ("59"->ReBigNumber.fromString, "test3"),
      ]);
    let check = ((n1, s1), (n2, s2)) =>
      n1->ReBigNumber.isEqualTo(n2) && s1 == s2;
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse mergeRows", ({expectTrue}) => {
    let value = "4235, test, test2";
    let encoding =
      Encodings.(merge_rows(tup2(number, string), cell(string))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([(("4235"->ReBigNumber.fromString, "test"), "test2")]);
    let check = (((n1, s1), s1'), ((n2, s2), s2')) =>
      n1->ReBigNumber.isEqualTo(n2) && s1 == s2 && s1' == s2';
    expectTrue(checkResultList(parsed, expected, check));
  });
});

describe("CSV.Or", ({test}) => {
  test("Parse number or string", ({expectTrue}) => {
    let value = "58\ntest";
    let encoding = Encodings.(or_(cell(number), cell(string))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([`Left("58"->ReBigNumber.fromString), `Right("test")]);
    let check = (v1, v2) =>
      switch (v1, v2) {
      | (`Left(n1), `Left(n2)) => n1->ReBigNumber.isEqualTo(n2)
      | (`Right(s1), `Right(s2)) => s1 == s2
      | _ => false
      };
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse rows with different lengths", ({expectTrue}) => {
    let value = "58, 12\ntest";
    let encoding =
      Encodings.(or_(tup2(number, number), cell(string))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([
        `Left(("58"->ReBigNumber.fromString, "12"->ReBigNumber.fromString)),
        `Right("test"),
      ]);
    let check = (v1, v2) =>
      switch (v1, v2) {
      | (`Left(n1, n1'), `Left(n2, n2')) =>
        n1->ReBigNumber.isEqualTo(n2) && n1'->ReBigNumber.isEqualTo(n2')
      | (`Right(s1), `Right(s2)) => s1 == s2
      | _ => false
      };
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse rows with null", ({expectTrue}) => {
    let value = "58, 12\n57\n";
    let encoding =
      Encodings.(merge_rows(or_(cell(number), null), cell(number))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected = Error(Encodings.IllformedEncoding);
    expectTrue(parsed == expected);
  });

  test("Parse rows with null at the end", ({expectTrue}) => {
    let value = "58, 12\n57\n";
    let encoding =
      Encodings.(merge_rows(cell(number), or_(cell(number), null))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([
        ("58"->ReBigNumber.fromString, `Left("12"->ReBigNumber.fromString)),
        ("57"->ReBigNumber.fromString, `Right()),
      ]);
    let check = ((n1, or1), (n2, or2)) => {
      let b = n1->ReBigNumber.isEqualTo(n2);
      switch (or1, or2) {
      | (`Left(n1'), `Left(n2')) => b && n1'->ReBigNumber.isEqualTo(n2')
      | (`Right (), `Right ()) => b
      | _ => false
      };
    };
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Parse rows with multiple or", ({expectTrue}) => {
    let value = "58\ntest\ntrue\n34\n22\nfalse";
    let encoding =
      Encodings.(or_(cell(number), or_(cell(bool), cell(string)))->mkRow);
    let parsed =
      encoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let expected =
      Ok([
        `Left("58"->ReBigNumber.fromString),
        `Right(`Right("test")),
        `Right(`Left(true)),
        `Left("34"->ReBigNumber.fromString),
        `Left("22"->ReBigNumber.fromString),
        `Right(`Left(false)),
      ]);
    let check = (or1, or2) => {
      switch (or1, or2) {
      | (`Left(n1'), `Left(n2')) => n1'->ReBigNumber.isEqualTo(n2')
      | (`Right(or1'), `Right(or2')) => or1' == or2'
      | _ => false
      };
    };
    expectTrue(checkResultList(parsed, expected, check));
  });
});

describe("CSV.Batch.v1", ({test}) => {
  let addr = Encodings.custom(~conv=PublicKeyHash.buildImplicit);
  let token = Encodings.custom(~conv=PublicKeyHash.buildContract);

  let rowEncoding =
    Encodings.(mkRow(tup4(addr, number, opt(token), opt(number))));

  let check = ((pkh1, am1, tk1, id1), (pkh2, am2, tk2, id2)) => {
    let checkIds = (id1, id2) =>
      switch (id1, id2) {
      | (Some(id1), Some(id2)) => id1->ReBigNumber.isEqualTo(id2)
      | (None, None) => true
      | _ => false
      };

    let checkToken = (tk1, tk2) =>
      switch (tk1, tk2) {
      | (Some(tk1), Some(tk2)) => tk1 == tk2
      | (None, None) => true
      | _ => false
      };

    pkh1 == pkh2
    && am1->ReBigNumber.isEqualTo(am2)
    && checkToken(tk1, tk2)
    && checkIds(id1, id2);
  };

  test("Tez row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let tez = "1.23456"->ReBigNumber.fromString;
    let expected = Ok([(pkh, tez, None, None)]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("FA1.2 row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let expected = Ok([(pkh, amount, Some(tk), None)]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("FA2 row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let tokenId = "2"->ReBigNumber.fromString;
    let expected = Ok([(pkh, amount, Some(tk), Some(tokenId))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Multiple rows", ({expectTrue}) => {
    let value = {|tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456
       tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2
       tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton
|};
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let tez = "1.23456"->ReBigNumber.fromString;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let tokenId = "2"->ReBigNumber.fromString;
    let expected =
      Ok([
        (pkh, tez, None, None),
        (pkh, amount, Some(tk), Some(tokenId)),
        (pkh, amount, Some(tk), None),
      ]);
    expectTrue(checkResultList(parsed, expected, check));
  });
});

describe("CSV.Batch.v1 with or", ({test}) => {
  let addr = Encodings.custom(~conv=PublicKeyHash.buildImplicit);
  let token = Encodings.custom(~conv=PublicKeyHash.buildContract);

  let rowEncoding =
    Encodings.(
      mkRow(
        merge_rows(
          tup2(addr, number),
          or_(merge_rows(cell(token), or_(cell(number), null)), null),
        ),
      )
    );

  let check = (((pkh1, am1), tk1), ((pkh2, am2), tk2)) => {
    let b = pkh1 == pkh2 && am1->ReBigNumber.isEqualTo(am2);
    let checkIds = (id1, id2) =>
      switch (id1, id2) {
      | (`Left(id1), `Left(id2)) => id1->ReBigNumber.isEqualTo(id2)
      | (`Right (), `Right ()) => true
      | _ => false
      };

    switch (tk1, tk2) {
    | (`Left(c1, id1), `Left(c2, id2)) =>
      b && c1 == c2 && checkIds(id1, id2)
    | (`Right (), `Right ()) => b
    | _ => false
    };
  };

  test("Tez row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let tez = "1.23456"->ReBigNumber.fromString;
    let expected = Ok([((pkh, tez), `Right())]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("FA1.2 row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let expected = Ok([((pkh, amount), `Left((tk, `Right())))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("FA2 row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let tokenId = "2"->ReBigNumber.fromString;
    let expected = Ok([((pkh, amount), `Left((tk, `Left(tokenId))))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Multiple rows", ({expectTrue}) => {
    let value = {|tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456
       tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2
       tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton
|};
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let tez = "1.23456"->ReBigNumber.fromString;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let tokenId = "2"->ReBigNumber.fromString;
    let expected =
      Ok([
        ((pkh, tez), `Right()),
        ((pkh, amount), `Left((tk, `Left(tokenId)))),
        ((pkh, amount), `Left((tk, `Right()))),
      ]);

    expectTrue(checkResultList(parsed, expected, check));
  });
});

describe("CSV.Batch.v2", ({test}) => {
  let rowEncoding = CSVEncoding.rowEncoding;

  let checkTransfer = (((pkh1, am1), tk1), ((pkh2, am2), tk2)) => {
    let b = pkh1 == pkh2 && am1->ReBigNumber.isEqualTo(am2);
    let checkIds = (id1, id2) =>
      switch (id1, id2) {
      | (Some(id1), Some(id2)) => id1->ReBigNumber.isEqualTo(id2)
      | (None, None) => true
      | _ => false
      };

    switch (tk1, tk2) {
    | (`Left(c1, id1), `Left(c2, id2)) =>
      b && c1 == c2 && checkIds(id1, id2)
    | (`Right (), `Right ()) => b
    | _ => false
    };
  };

  let checkContractCall = ((kt1, etp1, pld1, am1), (kt2, etp2, pld2, am2)) => {
    let checkAmounts = (am1, am2) =>
      switch (am1, am2) {
      | (Some(am1), Some(am2)) => am1->ReBigNumber.isEqualTo(am2)
      | (None, None) => true
      | _ => false
      };
    kt1 == kt2 && etp1 == etp2 && pld1 == pld2 && checkAmounts(am1, am2);
  };

  let check = (r1, r2) =>
    switch (r1, r2) {
    | (`Left(tr1), `Left(tr2)) => checkTransfer(tr1, tr2)
    | (`Right(cc1), `Right(cc2)) => checkContractCall(cc1, cc2)
    | _ => false
    };

  test("Tez row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let tez = "1.23456"->ReBigNumber.fromString;
    let expected = Ok([`Left(((pkh, tez), `Right()))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("FA1.2 row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let expected = Ok([`Left(((pkh, amount), `Left((tk, None))))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("FA2 row", ({expectTrue}) => {
    let value = "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2";
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let tokenId = "2"->ReBigNumber.fromString;
    let expected =
      Ok([`Left(((pkh, amount), `Left((tk, Some(tokenId)))))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Contract row", ({expectTrue}) => {
    let value = {|KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,default,(Pair 2 3),2|};
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let kt =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let entrypoint = "default";
    let micheline =
      "(Pair 2 3)"
      ->ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline
      ->Result.getExn;
    let amount = "2"->ReBigNumber.fromString;
    let expected = Ok([`Right((kt, entrypoint, micheline, Some(amount)))]);
    expectTrue(checkResultList(parsed, expected, check));
  });

  test("Multiple rows", ({expectTrue}) => {
    let value = {|tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1.23456
       tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,2
       tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2,1000,KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton
       KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,default,(Pair 2 3),1000
       KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton,default,(Pair 2 3)
|};
    let parsed =
      rowEncoding->Result.flatMap(encoding => parseCSV(value, encoding));
    let pkh =
      "tz1Z3JYEXYs88wAdaB6WW8H9tSRVxwuzEQz2"
      ->PublicKeyHash.buildImplicit
      ->Result.getExn;
    let tez = "1.23456"->ReBigNumber.fromString;
    let amount = "1000"->ReBigNumber.fromString;
    let tk =
      "KT1RJ6PbjHpwc3M5rw5s2Nbmefwbuwbdxton"
      ->PublicKeyHash.buildContract
      ->Result.getExn;
    let tokenId = "2"->ReBigNumber.fromString;
    let entrypoint = "default";
    let micheline =
      "(Pair 2 3)"
      ->ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline
      ->Result.getExn;
    let expected =
      Ok([
        `Left(((pkh, tez), `Right())),
        `Left(((pkh, amount), `Left((tk, Some(tokenId))))),
        `Left(((pkh, amount), `Left((tk, None)))),
        `Right((tk, entrypoint, micheline, Some(amount))),
        `Right((tk, entrypoint, micheline, None)),
      ]);

    expectTrue(checkResultList(parsed, expected, check));
  });
});
