import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {convertTimeTo12, convertTimeTo24, isNullOrUndefinedOrEmpty} from "../../../services/helper";
import {ComboBoxItem} from "../../../models/combo-box.model";
import {EffectModel} from "../../../models/effect.model";
import { MatButtonModule } from '@angular/material/button';
import { MatOptionModule } from '@angular/material/core';
import {NgClass, NgStyle} from '@angular/common';
import { MatSelectModule } from '@angular/material/select';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { NgxMatTimepickerModule } from 'ngx-mat-timepicker';
import { FormsModule } from '@angular/forms';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import {MatSlideToggleChange, MatSlideToggleModule} from "@angular/material/slide-toggle";
import {MatTooltipModule} from "@angular/material/tooltip";
import {MatCheckboxModule} from "@angular/material/checkbox";
import {Base} from "../../base.class";

@Component({
    selector: 'app-tab-modes',
    templateUrl: './tab-modes.component.html',
    styleUrls: ['./tab-modes.component.scss'],
    standalone: true,
  imports: [
    MatFormFieldModule,
    MatInputModule,
    FormsModule,
    NgxMatTimepickerModule,
    DisableControlDirective,
    MatSelectModule,
    MatOptionModule,
    NgStyle,
    MatButtonModule,
    MatSlideToggleModule,
    MatTooltipModule,
    NgClass,
    MatCheckboxModule,
  ],
})
export class TabModesComponent extends Base implements OnInit, OnDestroy {

  public mode1_time: string = '00:00';
  public mode1_effect = -3;
  public mode2_time: string = '00:00';
  public mode2_effect = -3;
  public mode3_time: string = '00:00';
  public mode3_effect = -3;
  public mode4_time: string = '00:00';
  public mode4_effect = -3;
  public mode5_time: string = '00:00';
  public mode5_effect = -3;
  public mode6_time: string = '00:00';
  public mode6_effect = -3;

  public effects: ComboBoxItem[] = [];

  public supportWeather: boolean = false;
  public supportAuxLine: boolean = false;

  public time12H: boolean = false;           // false: 24H;  true: 12H

  public isMode1AuxUse: boolean = false;
  public isMode1AuxOn: boolean = false;
  public isMode2AuxUse: boolean = false;
  public isMode2AuxOn: boolean = false;
  public isMode3AuxUse: boolean = false;
  public isMode3AuxOn: boolean = false;
  public isMode4AuxUse: boolean = false;
  public isMode4AuxOn: boolean = false;
  public isMode5AuxUse: boolean = false;
  public isMode5AuxOn: boolean = false;
  public isMode6AuxUse: boolean = false;
  public isMode6AuxOn: boolean = false;

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService
  ) {
    super();
  }

  ngOnInit() {

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          const request = 'WZ|C12|AM1T|AM1A|AM2T|AM2A|AM3T|AM3A|AM4T|AM4A|T1|AM5A|T2|AM6A|PZ2|FG';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'AM1T': {
              this.mode1_time = this.managementService.state.mode1_time.replace(' ', ':');
              break;
            }
            case 'AM1A':
              this.mode1_effect = this.managementService.state.mode1_effect;
              break;
            case 'AM2T': {
              this.mode2_time = this.managementService.state.mode2_time.replace(' ', ':');
              break;
            }
            case 'AM2A':
              this.mode2_effect = this.managementService.state.mode2_effect;
              break;
            case 'AM3T': {
              this.mode3_time = this.managementService.state.mode3_time.replace(' ', ':');
              break;
            }
            case 'AM3A':
              this.mode3_effect = this.managementService.state.mode3_effect;
              break;
            case 'AM4T': {
              this.mode4_time = this.managementService.state.mode4_time.replace(' ', ':');
              break;
            }
            case 'AM4A':
              this.mode4_effect = this.managementService.state.mode4_effect;
              break;
            case 'AM5A':
              this.mode5_effect = this.managementService.state.mode5_effect;
              break;
            case 'AM6A':
              this.mode6_effect = this.managementService.state.mode6_effect;
              break;
            case 'T1': {
              this.mode5_time = this.time12H
                ? convertTimeTo12(this.managementService.state.time_sunrise)
                : this.managementService.state.time_sunrise;
              break;
            }
            case 'T2': {
              this.mode6_time = this.time12H
                ? convertTimeTo12(this.managementService.state.time_sunset)
                : this.managementService.state.time_sunset;
              break;
            }
            case 'WZ': {
              this.supportWeather = this.managementService.state.supportWeather;
              break;
            }
            case 'PZ2': {
              this.supportAuxLine = this.managementService.state.supportAuxPower;
              break;
            }
            case 'FG': {
              const auxPowerMask = this.managementService.state.auxPowerModes;
              this.isMode1AuxUse = (auxPowerMask & 1) > 0;
              this.isMode1AuxOn = (auxPowerMask & 2) > 0;
              this.isMode2AuxUse = (auxPowerMask & 4) > 0;
              this.isMode2AuxOn = (auxPowerMask & 8) > 0;
              this.isMode3AuxUse = (auxPowerMask & 16) > 0;
              this.isMode3AuxOn = (auxPowerMask & 32) > 0;
              this.isMode4AuxUse = (auxPowerMask & 64) > 0;
              this.isMode4AuxOn = (auxPowerMask & 128) > 0;
              this.isMode5AuxUse = (auxPowerMask & 256) > 0;
              this.isMode5AuxOn = (auxPowerMask & 512) > 0;
              this.isMode6AuxUse = (auxPowerMask & 1024) > 0;
              this.isMode6AuxOn = (auxPowerMask & 2048) > 0;
              break;
            }
            case 'C12': {
              this.time12H = this.managementService.state.time12h;
              break;
            }
          }
        }
      });

    this.managementService.effects$
      .pipe(takeUntil(this.destroy$)).subscribe((effects: EffectModel[]) => {
      this.effects = [];
      this.effects.push({value: -3, displayText: this.L.$('Нет действия')});
      this.effects.push({value: -2, displayText: this.L.$('Выключить')});
      this.effects.push({value: -1, displayText: this.L.$('Ночные часы')});
      this.effects.push({value: 0, displayText: this.L.$('Демо режим')});
      let idx = 1;
      for (const effect of effects) {
        this.effects.push({value: idx++, displayText: effect.name});
      }
    });
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  applyTimeModes($event: MouseEvent) {

    // ----------------------------------------------------
    // 22 - настройки включения режимов матрицы в указанное время NN5 - Действие на "Рассвет", NN6 - действие на "Закат"
    // - $22 HH1 MM1 NN1 HH2 MM2 NN2 HH3 MM3 NN3 HH4 MM4 NN4 NN5 NN6;
    //     HHn - час срабатывания
    //     MMn - минуты срабатывания
    //     NNn - эффект: -3 - выключено; -2 - выключить матрицу; -1 - ночные часы; 0 - случайный режим и далее по кругу; 1 и далее - список режимов EFFECT_LIST
    // ----------------------------------------------------

    let time = convertTimeTo24(this.mode1_time).split(':');
    this.managementService.state.mode1_time = `${time[0]} ${time[1]}`;
    const HH1 = Number(time[0]);
    const MM1 = Number(time[1]);
    const NN1 = this.mode1_effect;

    time = convertTimeTo24(this.mode2_time).split(':');
    this.managementService.state.mode2_time = `${time[0]} ${time[1]}`;
    const HH2 = Number(time[0]);
    const MM2 = Number(time[1]);
    const NN2 = this.mode2_effect;

    time = convertTimeTo24(this.mode3_time).split(':');
    this.managementService.state.mode3_time = `${time[0]} ${time[1]}`;
    const HH3 = Number(time[0]);
    const MM3 = Number(time[1]);
    const NN3 = this.mode3_effect;

    time = convertTimeTo24(this.mode4_time).split(':');
    this.managementService.state.mode4_time = `${time[0]} ${time[1]}`;
    const HH4 = Number(time[0]);
    const MM4 = Number(time[1]);
    const NN4 = this.mode4_effect;

    const NN5 = this.mode5_effect;
    const NN6 = this.mode6_effect;
    this.socketService.sendText(`$22 ${HH1} ${MM1} ${NN1} ${HH2} ${MM2} ${NN2} ${HH3} ${MM3} ${NN3} ${HH4} ${MM4} ${NN4} ${NN5} ${NN6};`);

    if (this.supportAuxLine) {
      let value = 0;
      if (this.isMode1AuxUse) value += 1;
      if (this.isMode1AuxOn) value += 2;
      if (this.isMode2AuxUse) value += 4;
      if (this.isMode2AuxOn) value += 8;
      if (this.isMode3AuxUse) value += 16;
      if (this.isMode3AuxOn) value += 32;
      if (this.isMode4AuxUse) value += 64;
      if (this.isMode4AuxOn) value += 128;
      if (this.isMode5AuxUse) value += 256;
      if (this.isMode5AuxOn) value += 512;
      if (this.isMode6AuxUse) value += 1024;
      if (this.isMode6AuxOn) value += 2048;
      this.socketService.sendText(`$23 20 ${value};`);
    }
  }

  toggleChange($event: MatSlideToggleChange, number: number) {
    if ($event.checked) return;
    switch (number) {
      case 1:
        this.isMode1AuxOn = false;
        break;
      case 2:
        this.isMode2AuxOn = false;
        break;
      case 3:
        this.isMode3AuxOn = false;
        break;
      case 4:
        this.isMode4AuxOn = false;
        break;
      case 5:
        this.isMode5AuxOn = false;
        break;
      case 6:
        this.isMode6AuxOn = false;
        break;
    }
  }

}
