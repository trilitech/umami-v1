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

module Dark = {
  let maxEmphasis = "#FFF";
  let highEmphasis = "rgba(255,255,255,0.87)";
  let mediumEmphasis = "rgba(255,255,255,0.6)";
  let disabled = "rgba(255,255,255,0.38)";

  // for amount data
  let positive = "#7cd6ae";
  let negative = "#f97977";

  let primary = "#f74f18";
  let primaryOutline = "rgba(247, 79, 24, 0.38)";

  let surface = "#a4a4a4";
  let surfaceOutline = "rgba(164, 164, 164, 0.38)";

  let background = "#121212";
  let barBackground = "#212121";
  let backgroundMediumEmphasis = "rgba(0,0,0,0.74)";
  let scrim = "rgba(92,92,92,0.32)";
  let cardBackground = "#000";

  let elevatedBackground = "#212121";

  let statePressed = "rgba(255, 255, 255, 0.28)";
  let stateHovered = "rgba(255, 255, 255, 0.16)";
  let stateFocusedOutline = "rgba(255, 255, 255, 0.38)";
  let stateActive = "rgba(255, 255, 255, 0.08)";
  let stateDisabled = "rgba(255, 255, 255, 0.12)";
  let stateSmallHovered = "rgba(255, 255, 255, 0.04)";

  // for application state like form
  let error = "#d35050"; //red
  let valid = "#7cd6ae"; //green
};

module Light = {
  let maxEmphasis = "#000";
  let highEmphasis = "rgba(0,0,0,0.87)";
  let mediumEmphasis = "rgba(0,0,0,0.6)";
  let disabled = "rgba(0,0,0,0.38)";

  // for amount data
  let positive = "#55a388";
  let negative = "#d46250";

  let primary = "#f74f18";
  let primaryOutline = "rgba(247, 79, 24, 0.38)";

  let surface = "#a4a4a4";
  let surfaceOutline = "rgba(164, 164, 164, 0.38)";

  let background = "#f9f9f9";
  let barBackground = "#FFF";
  let backgroundMediumEmphasis = "rgba(255,255,255,0.74)";
  let cardBackground = "#fff";
  let scrim = "rgba(153,145,134,0.32)";

  let statePressed = "rgba(0, 0, 0, 0.2)";
  let stateHovered = "rgba(0, 0, 0, 0.12)";
  let stateFocusedOutline = "rgba(0, 0, 0, 0.38)";
  let stateActive = "rgba(0, 0, 0, 0.08)";
  let stateDisabled = "rgba(0, 0, 0, 0.12)";
  let stateSmallHovered = "rgba(0, 0, 0, 0.04)";

  // for application state like form
  let error = "#bb3838"; //red
  let valid = "#55a388"; //green
};
