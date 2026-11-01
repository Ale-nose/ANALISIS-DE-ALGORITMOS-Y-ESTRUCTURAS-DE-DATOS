// Copyright Alejandro, Ashley y Kevin
#include "MenuTheme.hpp"

#include <utility>
#include <vector>

namespace {

using Token = std::pair<const char*, QString>;

/**
 * @brief Replaces every <NAME> placeholder in a stylesheet with its value.
 * Named placeholders avoid the fragile numbering of QString::arg (%1, %2...):
 * adding a line to the stylesheet never requires renumbering the others.
 * @param style Stylesheet text containing <NAME> placeholders.
 * @param tokens Pairs of placeholder name (without brackets) and its value.
 * @return The stylesheet with all the placeholders replaced.
 */
QString applyTokens(QString style, const std::vector<Token>& tokens) {
  for (const auto& [name, value] : tokens) {
    style.replace(QString("<%1>").arg(name), value);
  }
  return style;
}

}  // namespace

QString buildStyleSheet() {
  const QString style = R"(
    QWidget {
      background-color: <COLOR_BACKGROUND>;
      color: <COLOR_TEXT>;
      font-family: "<UI_FONT_FAMILY>";
      font-size: <UI_FONT_SIZE>px;
    }
    QLabel#titleLabel {
      font-size: <UI_TITLE_FONT_SIZE>px;
      font-weight: bold;
      color: <COLOR_TITLE>;
    }
    QLabel#subtitleLabel {
      font-size: <UI_SUBTITLE_FONT_SIZE>px;
      color: <COLOR_TEXT_MUTED>;
    }
    QGroupBox {
      background-color: <COLOR_PANEL>;
      border: <UI_BORDER_WIDTH>px solid <COLOR_BORDER>;
      border-radius: <UI_BORDER_RADIUS>px;
      margin-top: <UI_GROUP_MARGIN_TOP>px;
      padding: <UI_GROUP_PADDING>px;
    }
    QGroupBox::title {
      subcontrol-origin: margin;
      left: <UI_GROUP_TITLE_LEFT>px;
      padding: 0 <UI_GROUP_TITLE_PADDING>px;
      color: <COLOR_ACCENT>;
      font-weight: bold;
    }
    QGroupBox QRadioButton, QGroupBox QCheckBox {
      background-color: transparent;
      spacing: <UI_OPTION_SPACING>px;
      padding: <UI_OPTION_PADDING>px;
    }
    QRadioButton::indicator, QCheckBox::indicator {
      width: <UI_INDICATOR_SIZE>px;
      height: <UI_INDICATOR_SIZE>px;
      border: <UI_INDICATOR_BORDER_WIDTH>px solid <COLOR_INDICATOR_BORDER>;
      background-color: <COLOR_INDICATOR_BG>;
    }
    QRadioButton::indicator {
      border-radius: <UI_RADIO_RADIUS>px;
    }
    QCheckBox::indicator {
      border-radius: <UI_CHECKBOX_RADIUS>px;
    }
    QRadioButton::indicator:hover, QCheckBox::indicator:hover {
      border-color: <COLOR_ACCENT>;
    }
    QRadioButton::indicator:checked {
      border-color: <COLOR_ACCENT>;
      background-color: qradialgradient(cx: 0.5, cy: 0.5, radius: 0.5,
        fx: 0.5, fy: 0.5, stop: 0 <COLOR_ACCENT>, stop: 0.5 <COLOR_ACCENT>,
        stop: 0.6 <COLOR_INDICATOR_BG>, stop: 1 <COLOR_INDICATOR_BG>);
    }
    QCheckBox::indicator:checked {
      border-color: <COLOR_ACCENT>;
      background-color: <COLOR_ACCENT>;
    }
    QPushButton#startButton {
      background-color: <COLOR_BUTTON>;
      color: <COLOR_BUTTON_TEXT>;
      border: none;
      border-radius: <UI_BORDER_RADIUS>px;
      padding: <UI_BUTTON_PADDING_VERTICAL>px <UI_BUTTON_PADDING_HORIZONTAL>px;
      font-size: <UI_BUTTON_FONT_SIZE>px;
      font-weight: bold;
    }
    QPushButton#startButton:hover {
      background-color: <COLOR_BUTTON_HOVER>;
    }
  )";

  return applyTokens(
      style,
      {
          {"COLOR_BACKGROUND", COLOR_BACKGROUND},
          {"COLOR_PANEL", COLOR_PANEL},
          {"COLOR_BORDER", COLOR_BORDER},
          {"COLOR_INDICATOR_BORDER", COLOR_INDICATOR_BORDER},
          {"COLOR_INDICATOR_BG", COLOR_INDICATOR_BG},
          {"COLOR_TEXT", COLOR_TEXT},
          {"COLOR_TEXT_MUTED", COLOR_TEXT_MUTED},
          {"COLOR_TITLE", COLOR_TITLE},
          {"COLOR_ACCENT", COLOR_ACCENT},
          {"COLOR_BUTTON", COLOR_BUTTON},
          {"COLOR_BUTTON_HOVER", COLOR_BUTTON_HOVER},
          {"COLOR_BUTTON_TEXT", COLOR_BUTTON_TEXT},
          {"UI_FONT_FAMILY", UI_FONT_FAMILY},
          {"UI_FONT_SIZE", QString::number(UI_FONT_SIZE)},
          {"UI_TITLE_FONT_SIZE", QString::number(UI_TITLE_FONT_SIZE)},
          {"UI_SUBTITLE_FONT_SIZE", QString::number(UI_SUBTITLE_FONT_SIZE)},
          {"UI_BUTTON_FONT_SIZE", QString::number(UI_BUTTON_FONT_SIZE)},
          {"UI_BORDER_RADIUS", QString::number(UI_BORDER_RADIUS)},
          {"UI_BORDER_WIDTH", QString::number(UI_BORDER_WIDTH)},
          {"UI_GROUP_MARGIN_TOP", QString::number(UI_GROUP_MARGIN_TOP)},
          {"UI_GROUP_PADDING", QString::number(UI_GROUP_PADDING)},
          {"UI_GROUP_TITLE_LEFT", QString::number(UI_GROUP_TITLE_LEFT)},
          {"UI_GROUP_TITLE_PADDING", QString::number(UI_GROUP_TITLE_PADDING)},
          {"UI_OPTION_SPACING", QString::number(UI_OPTION_SPACING)},
          {"UI_OPTION_PADDING", QString::number(UI_OPTION_PADDING)},
          {"UI_INDICATOR_SIZE", QString::number(UI_INDICATOR_SIZE)},
          {"UI_INDICATOR_BORDER_WIDTH",
           QString::number(UI_INDICATOR_BORDER_WIDTH)},
          {"UI_CHECKBOX_RADIUS", QString::number(UI_CHECKBOX_RADIUS)},
          {"UI_RADIO_RADIUS",
           QString::number(UI_INDICATOR_SIZE / 2 + UI_INDICATOR_BORDER_WIDTH)},
          {"UI_BUTTON_PADDING_VERTICAL",
           QString::number(UI_BUTTON_PADDING_VERTICAL)},
          {"UI_BUTTON_PADDING_HORIZONTAL",
           QString::number(UI_BUTTON_PADDING_HORIZONTAL)},
      });
}
