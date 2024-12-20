import {Component, Input, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, takeUntil} from 'rxjs';
import {WebsocketService} from '../../services/websocket/websocket.service';
import { MatSliderModule } from '@angular/material/slider';
import {Base} from "../base.class";

@Component({
    selector: 'app-brightness-slider',
    templateUrl: './brightness-slider.component.html',
    styleUrls: ['./brightness-slider.component.scss'],
    standalone: true,
    imports: [MatSliderModule]
})
export class BrightnessSliderComponent extends Base implements OnInit, OnDestroy {

  @Input() disabled: boolean = false;
  @Input() value: number = -1;

  private valueChanged$ = new BehaviorSubject(this.value);

  public get percent(): number {
    return Math.round(this.value * 100 / 255);
  }

  constructor(public socketService: WebsocketService) {
    super();
  }

  ngOnInit(): void {
    this.valueChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (value >= 0) {
          this.value = value;
          // $4 0 value   установить текущий уровень общей яркости
          const command = `$4 0 ${value};`;
          this.socketService.sendText(command);
        }
      });
  }

  formatLabel(percent: number) {
    return percent + (percent === 100 ? '' : '%');
  }

  valueChanged($event: number | null) {
    const value = Math.round(Number($event) * 255 / 100);
    this.valueChanged$.next(value);
  }

}
