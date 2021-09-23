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
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
    ) => {
  <Svg viewBox="0 0 87 20" ?width ?height>
    <G fillRule=`evenodd>
      <G fillRule=`nonzero>
        <G>
          <G ?fill>
            <Path
              d="M8.607 3.919v8.787H6.649v-1.119c-.33.395-.74.7-1.234.913-.494.214-1.026.321-1.597.321-1.174 0-2.098-.326-2.772-.979C.37 11.19.034 10.222.034 8.938v-5.02H2.09v4.74c0 .79.178 1.38.534 1.769.357.39.864.584 1.522.584.735 0 1.32-.228 1.753-.683.433-.455.65-1.11.65-1.966V3.919h2.057zM25.441 4.783c.647.641.971 1.604.971 2.888v5.035h-2.057V7.934c0-.768-.17-1.347-.51-1.736-.34-.39-.829-.584-1.465-.584-.69 0-1.245.227-1.662.683-.417.455-.625 1.105-.625 1.95v4.459h-2.057V7.934c0-.768-.17-1.347-.51-1.736-.34-.39-.829-.584-1.465-.584-.702 0-1.259.225-1.67.674-.411.45-.617 1.103-.617 1.959v4.459h-2.057V3.919h1.958v1.119c.33-.395.74-.697 1.234-.905.494-.209 1.043-.313 1.646-.313.658 0 1.242.123 1.753.37.51.247.913.612 1.21 1.094.361-.46.827-.82 1.398-1.077.57-.258 1.201-.387 1.892-.387 1.108 0 1.986.32 2.633.963zM35.783 4.75c.686.62 1.029 1.555 1.029 2.805v5.151H34.87v-1.07c-.252.385-.612.678-1.078.88-.466.204-1.028.305-1.686.305-.659 0-1.235-.112-1.728-.337-.494-.225-.875-.538-1.144-.938-.269-.4-.403-.853-.403-1.358 0-.79.293-1.423.88-1.9.587-.478 1.511-.716 2.773-.716h2.271V7.44c0-.614-.184-1.086-.551-1.415-.368-.33-.914-.494-1.638-.494-.493 0-.979.077-1.456.23-.477.154-.88.368-1.21.642l-.806-1.497c.461-.351 1.015-.62 1.662-.806.647-.187 1.333-.28 2.057-.28 1.295 0 2.285.31 2.97.93zm-1.876 6.22c.4-.236.683-.573.848-1.012v-1.02h-2.123c-1.185 0-1.777.39-1.777 1.168 0 .373.148.67.444.889.296.22.708.329 1.234.329.516 0 .974-.118 1.374-.354zM53.58 4.783c.647.641.971 1.604.971 2.888v5.035h-2.057V7.934c0-.768-.17-1.347-.51-1.736-.34-.39-.828-.584-1.465-.584-.69 0-1.245.227-1.662.683-.417.455-.625 1.105-.625 1.95v4.459h-2.057V7.934c0-.768-.17-1.347-.51-1.736-.34-.39-.828-.584-1.465-.584-.702 0-1.258.225-1.67.674-.411.45-.617 1.103-.617 1.959v4.459h-2.057V3.919h1.958v1.119c.33-.395.74-.697 1.235-.905.493-.209 1.042-.313 1.645-.313.658 0 1.243.123 1.753.37.51.247.913.612 1.21 1.094.361-.46.827-.82 1.398-1.077.57-.258 1.201-.387 1.892-.387 1.108 0 1.986.32 2.633.963zM57.694 2.117c-.252-.236-.378-.53-.378-.88 0-.352.126-.645.378-.881s.565-.354.938-.354.685.113.938.338c.252.225.378.507.378.847 0 .362-.123.666-.37.913s-.562.37-.946.37c-.373 0-.686-.117-.938-.353zm-.099 1.802h2.057v8.787h-2.057V3.919z"
              transform="translate(-110 -20) translate(110 20) translate(26.713 1.646)"
            />
          </G>
          <G>
            <Path
              fill="#000"
              d="M14.881.292H4.405C2.137.292.292 2.137.292 4.405v10.476c0 2.268 1.845 4.113 4.113 4.113h10.476c1.099 0 2.132-.427 2.909-1.204.777-.777 1.204-1.81 1.204-2.909V4.405c0-2.268-1.845-4.113-4.113-4.113z"
              transform="translate(-110 -20) translate(110 20)"
            />
            <G fill="#FFF" transform="translate(-110 -20) translate(110 20)">
              <Path
                d="M14.881 0H4.405C1.976 0 0 1.976 0 4.405v10.476c0 2.43 1.976 4.405 4.405 4.405h10.476c1.177 0 2.283-.458 3.115-1.29.832-.832 1.29-1.938 1.29-3.115V4.405C19.286 1.976 17.31 0 14.881 0zm3.821 14.881c0 1.02-.397 1.98-1.119 2.702s-1.681 1.12-2.702 1.12h-1.926v-2.728h1.926c1.177 0 2.283-.459 3.115-1.29.279-.28.515-.59.706-.922v1.118zm0-3.31c0 1.02-.397 1.979-1.119 2.7-.722.722-1.681 1.12-2.702 1.12H9.643c-.673 0-1.218-.546-1.218-1.218 0-.673.545-1.218 1.218-1.218h5.238c1.177 0 2.283-.458 3.115-1.29.279-.28.515-.59.706-.922v.827zm0-3.02c0 1.02-.397 1.98-1.119 2.7-.722.723-1.681 1.12-2.702 1.12h-1.926V9.643h1.926c1.788 0 3.238-1.45 3.238-3.237v-2c0-1.789-1.45-3.238-3.238-3.238H4.405c-1.788 0-3.237 1.45-3.237 3.237v2c0 1.789 1.45 3.238 3.237 3.238h7.966v2.728H9.643c-.995 0-1.802.807-1.802 1.802s.807 1.802 1.802 1.802h2.728v2.727H4.405c-2.107 0-3.821-1.714-3.821-3.82V4.404C.584 2.298 2.298.584 4.405.584h10.476c2.107 0 3.821 1.714 3.821 3.821v4.146z"
              />
              <Circle
                cx={9.643->Style.dp}
                cy={14.173->Style.dp}
                r={1.->Style.dp}
              />
            </G>
            <Path
              fill="#F74F18"
              d="M14.881 2.697c.77 0 1.423.511 1.635 1.213.03.1-.049.198-.153.194-1.462-.06-2.826-.504-3.994-1.234-.079-.05-.043-.173.051-.173h2.461zM11.34 2.862c1.377 1.066 3.078 1.732 4.928 1.822.18.009.322.155.322.335v1.153l-.003.178c-.002.13-.11.232-.24.228-3.085-.1-5.842-1.518-7.726-3.708-.059-.068-.01-.173.079-.173h2.158c.174 0 .344.058.483.165z"
              transform="translate(-110 -20) translate(110 20)"
            />
            <Path
              fill="#F74F18"
              d="M7.925 2.946c1.982 2.472 4.984 4.093 8.363 4.214.058.002.093.065.064.115-.298.502-.846.84-1.471.84h-2.902c-.11 0-.218-.021-.32-.064C9.125 6.995 6.991 5.165 5.552 2.86c-.044-.07.008-.162.09-.162h1.765c.202 0 .392.092.518.249z"
              transform="translate(-110 -20) translate(110 20)"
            />
            <Path
              fill="#F74F18"
              d="M4.405 2.697h.206c.102 0 .195.054.247.142 1.26 2.152 3.09 3.931 5.282 5.13.07.038.043.145-.037.145H7.445c-.314 0-.619-.109-.858-.312C5.194 6.614 4.011 5.185 3.105 3.58c-.06-.104-.044-.235.036-.324.313-.344.764-.56 1.264-.56zM5.854 7.934c.07.064.025.18-.07.18H4.386c-.934-.01-1.69-.772-1.69-1.708V4.289c0-.068.091-.094.127-.035.825 1.372 1.849 2.612 3.031 3.68z"
              transform="translate(-110 -20) translate(110 20)"
            />
          </G>
        </G>
      </G>
    </G>
  </Svg>;
};
